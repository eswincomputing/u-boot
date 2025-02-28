// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 */

#include <common.h>
#include <autoboot.h>
#include <bootretry.h>
#include <cli.h>
#include <command.h>
#include <console.h>
#include <env.h>
#include <fdtdec.h>
#include <hash.h>
#include <log.h>
#include <malloc.h>
#include <memalign.h>
#include <menu.h>
#include <post.h>
#include <stdio.h>
#include <time.h>
#include <asm/global_data.h>
#include <linux/delay.h>
#include <u-boot/sha256.h>
#include <bootcount.h>
#include <crypt.h>
#include <dm/ofnode.h>
#include <stdlib.h>
#include <linux/io.h>
#include <net.h>
#ifdef CONFIG_BOOT_ESWIN_VPU7702
#include <i2c.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

#define DELAY_STOP_STR_MAX_LENGTH 64

#ifndef DEBUG_BOOTKEYS
#define DEBUG_BOOTKEYS 0
#endif
#define debug_bootkeys(fmt, args...)		\
	debug_cond(DEBUG_BOOTKEYS, fmt, ##args)

/* Stored value of bootdelay, used by autoboot_command() */
static int stored_bootdelay;
static int menukey;

#if defined(CONFIG_AUTOBOOT_STOP_STR_CRYPT)
#define AUTOBOOT_STOP_STR_CRYPT	CONFIG_AUTOBOOT_STOP_STR_CRYPT
#else
#define AUTOBOOT_STOP_STR_CRYPT	""
#endif
#if defined(CONFIG_AUTOBOOT_STOP_STR_SHA256)
#define AUTOBOOT_STOP_STR_SHA256	CONFIG_AUTOBOOT_STOP_STR_SHA256
#else
#define AUTOBOOT_STOP_STR_SHA256	""
#endif

#ifdef CONFIG_AUTOBOOT_USE_MENUKEY
#define AUTOBOOT_MENUKEY CONFIG_AUTOBOOT_MENUKEY
#else
#define AUTOBOOT_MENUKEY 0
#endif
#ifdef CONFIG_BOOT_ESWIN_VPU7702
#define BOOT_SIGN	0xB00C
#define READY_SIGN	0x2EA1
#define TEST_REG0	0x51810668

#define AT24C_ADDR			      (0x50)
#define CARRIER_BOARD_INFO_EEPROM_MAIN_OFFSET 0
#define BD_TABLE_PHYS_ADDR		      0x104413000ULL

/* Define magic number for BoardInfo structure */
#define HARDWARE_BOARD_INFO_MAGIC_NUMBER 0x05454943

/* Define this to use mock board info instead of reading from EEPROM */
// #define USE_MOCK_BOARD_INFO

typedef struct {
	uint32_t magicNumber;
	uint8_t formatVersionNumber;
	uint16_t productIdentifier;
	uint8_t pcbRevision;
	uint8_t bomRevision;
	uint8_t bomVariant;
	uint8_t SomSerialNumber[18];
	uint8_t manufacturingTestStatus;

	uint8_t boardSerialNumber[32];
	uint8_t ethernetMAC1[6];
	uint8_t ethernetMAC2[6];
	uint8_t boardName[32];
	uint8_t productModel[32];
	uint8_t hardwareVersion[16];
	uint8_t softwareVersion[16];
	uint8_t stationIdentifier[32];
	uint8_t factoryInit[2];

	uint32_t crc32Checksum;
} BoardInfo;

/**
 * get_die_ordinary - Get the die ordinary value based on GPIO status
 *
 * Check GPIO58~61 to determine if this is DIE0
 * DIE0 is indicated by all GPIO58~61 being 0
 *
 * Return: 0 if DIE0, 1 if not DIE0
 */
int get_die_ordinary(void)
{
	int die_ordinary = 0;
#ifdef USE_MOCK_BOARD_INFO
	char *die_idx_str;

	die_idx_str = env_get("die_idx");

	if (die_idx_str == NULL) {
		printf("Error: die_idx environment variable not set.\n");
		return -1;
	}

	die_ordinary = simple_strtol(die_idx_str, NULL, 10);

	printf("Current die = %d\n", die_ordinary);
	return die_ordinary;
#else
	// Read die ordinary GPIO values
	int die_values[4] = { 0 };
	unsigned int die_gpio_pins[4] = { 58, 59, 60, 61 };
	int i;
	int ret;

	printf("Reading die ordinary GPIOs:\n");
	for (i = 0; i < 4; i++) {
		ret = gpio_request(die_gpio_pins[i], "die-ordinary");
		if (ret) {
			printf("Failed to request GPIO%d: %d\n", die_gpio_pins[i], ret);
			return ret;
		}

		ret = gpio_direction_input(die_gpio_pins[i]);
		if (ret) {
			printf("Failed to set GPIO%d as input: %d\n", die_gpio_pins[i], ret);
			gpio_free(die_gpio_pins[i]);
			return ret;
		}

		die_values[i] = gpio_get_value(die_gpio_pins[i]);

		gpio_free(die_gpio_pins[i]);
	}

	// Calculate die_ordinary from GPIO values
	die_ordinary = die_values[3] << 3 | die_values[2] << 2 | die_values[1] << 1 | die_values[0];

	// Ensure die_ordinary is within the valid range (0-9)
	if (die_ordinary > 9) {
		printf("die_ordinary is out of range\n");
		for (i = 0; i < 4; i++) {
			printf("GPIO%d value: %d\n", die_gpio_pins[i], die_values[i]);
		}
		return -1;
	}

	return die_ordinary;
#endif /* USE_MOCK_BOARD_INFO */
}

uint32_t calculate_crc32(const uint8_t *data, int length)
{
	uint32_t crc = 0xFFFFFFFF;
	int i, j;

	for (i = 0; i < length; i++) {
		crc ^= data[i];
		for (j = 0; j < 8; j++) {
			if (crc & 1)
				crc = (crc >> 1) ^ 0xEDB88320; // Polynomial in reverse bit order
			else
				crc = crc >> 1;
		}
	}

	return ~crc; // Final XOR value
}

#ifdef USE_MOCK_BOARD_INFO
/**
 * get_mock_board_info - Populate BoardInfo with mock data for testing
 *
 * This function creates a BoardInfo structure with predefined values
 * for testing when real EEPROM hardware is not available.
 *
 * @board_info: Pointer to BoardInfo structure to populate
 * Return: 0 on success, -1 on failure
 */
static int get_mock_board_info(BoardInfo *board_info)
{
	if (!board_info) {
		printf("%s: board_info is NULL\n", __func__);
		return -1;
	}

	/* Initialize entire structure to zeros */
	memset((uint8_t *)board_info, 0, sizeof(BoardInfo));

	/* Fill with required mock data */
	board_info->magicNumber = HARDWARE_BOARD_INFO_MAGIC_NUMBER; /* 0x05454943 */
	board_info->formatVersionNumber = 0x1;
	board_info->productIdentifier = 0x0;
	board_info->pcbRevision = 0x0; /* Set to 0x0 */
	board_info->bomRevision = 0x0; /* Set to 0x0 */
	board_info->bomVariant = 0x0; /* Set to 0x0 */

	/* Update SomSerialNumber to "TELEVPU225150001" */
	strncpy((char *)board_info->SomSerialNumber, "TELEVPU225150001",
		sizeof(board_info->SomSerialNumber) - 1);

	board_info->manufacturingTestStatus = 0x1;

	/* Calculate CRC32 checksum for the structure */
	board_info->crc32Checksum = calculate_crc32((uint8_t *)board_info, sizeof(BoardInfo) - 4);

	printf("Mock BoardInfo created successfully\n");
	return 0;
}
#endif /* USE_MOCK_BOARD_INFO */

static int get_board_info_from_eeprom(BoardInfo *board_info)
{
	uint32_t crc32Checksum;
	struct udevice *bus;
	struct udevice *dev;
	int ret;

	if (!board_info) {
		printf("%s: board_info is NULL\n", __func__);
		return -1;
	}

	ret = uclass_get_device_by_seq(UCLASS_I2C, 5, &bus);
	if (ret) {
		printf("%s: No bus %d\n", __func__, 5);
		return ret;
	}

	/* Use dm_i2c_probe instead of i2c_get_chip for initialization */
	ret = dm_i2c_probe(bus, AT24C_ADDR, 0, &dev);
	if (ret) {
		printf("%s: Failed to probe I2C device at address 0x%x, ret=%d\n", __func__,
		       AT24C_ADDR, ret);
		return ret;
	}

	ret = i2c_set_chip_offset_len(dev, 1);
	if (ret) {
		printf("%s: Failed to set chip offset length, ret=%d\n", __func__, ret);
		return -1;
	}

	memset((uint8_t *)board_info, 0, sizeof(BoardInfo));
	ret = dm_i2c_read(dev, CARRIER_BOARD_INFO_EEPROM_MAIN_OFFSET, (uint8_t *)board_info,
			  sizeof(BoardInfo));
	if (ret != 0) {
		printf("WARN: cannot read board info from EEPROM!\n");
		return -1;
	}
	crc32Checksum =
		calculate_crc32((uint8_t *)board_info, sizeof(BoardInfo) - sizeof(uint32_t));

	if (crc32Checksum != board_info->crc32Checksum) {
		printf("CRC32 checksum mismatch!\n");
		return -1;
	}

	if (board_info->magicNumber != HARDWARE_BOARD_INFO_MAGIC_NUMBER) {
		printf("Primary EEPROM section invalid...\n");
		return -1;
	}

	printf("SomSerialNumber: %s\n", board_info->SomSerialNumber);

	return 0;
}

/**
 * get_board_info - Get board info either from EEPROM or mock data
 *
 * This is a wrapper function that either gets real data from EEPROM
 * or uses mock data depending on compile-time configuration.
 *
 * @board_info: Pointer to BoardInfo structure to populate
 * Return: 0 on success, negative on failure
 */
static int get_board_info(BoardInfo *board_info)
{
	int ret;

#ifdef USE_MOCK_BOARD_INFO
	ret = get_mock_board_info(board_info);
	if (ret) {
		printf("Failed to get mock board info, ret=%d\n", ret);
		return ret;
	}
#else
	ret = get_board_info_from_eeprom(board_info);
	if (ret) {
		printf("Failed to read board info from EEPROM, ret=%d\n", ret);
		return ret;
	}
#endif /* USE_MOCK_BOARD_INFO */

	return 0;
}

static int save_board_info_to_fdt()
{
	BoardInfo board_info;
	void *bd_table_virt_addr;
	int ret;
	int die_ordinary;

	// Get die ordinary value
	die_ordinary = get_die_ordinary();
	if (die_ordinary == -1) {
		printf("Failed to get die ordinary value, ret=%d\n", die_ordinary);
		return -1;
	}

	bd_table_virt_addr = (void *)BD_TABLE_PHYS_ADDR;

	// Only DIE0 needs to initialize shared memory
	if (die_ordinary == 0) {
		printf("DIE0: Reading board info and initializing shared memory...\n");
		// Get BoardInfo (either from EEPROM or mock data)
		ret = get_board_info(&board_info);
		if (ret) {
			printf("Failed to get board info, ret=%d\n", ret);
			return ret;
		} else {
			memcpy((uint8_t *)bd_table_virt_addr, &board_info, sizeof(BoardInfo));
			printf("Successfully wrote BoardInfo to shared memory\n");
		}
	} else {
		printf("Not DIE0, skipping shared memory initialization\n");
	}

	// Write die_idx
	*((uint8_t *)bd_table_virt_addr + sizeof(BoardInfo)) = (uint8_t)die_ordinary;
	// read back check
	die_ordinary = *((uint8_t *)bd_table_virt_addr + sizeof(BoardInfo));
	printf("Read back die_ordinary: %d\n", die_ordinary);

	return 0;
}
#endif /* CONFIG_BOOT_ESWIN_VPU7702 */

/**
 * passwd_abort_crypt() - check for a crypt-style hashed key sequence to abort booting
 *
 * This checks for the user entering a password within a given time.
 *
 * The entered password is hashed via one of the crypt-style hash methods
 * and compared to the pre-defined value from either
 *   the environment variable "bootstopkeycrypt"
 * or
 *   the config value CONFIG_AUTOBOOT_STOP_STR_CRYPT
 *
 * In case the config value CONFIG_AUTOBOOT_NEVER_TIMEOUT has been enabled
 * this function never times out if the user presses the <Enter> key
 * before starting to enter the password.
 *
 * @etime: Timeout value ticks (stop when get_ticks() reachs this)
 * Return: 0 if autoboot should continue, 1 if it should stop
 */
static int passwd_abort_crypt(uint64_t etime)
{
	const char *crypt_env_str = env_get("bootstopkeycrypt");
	char presskey[DELAY_STOP_STR_MAX_LENGTH];
	u_int presskey_len = 0;
	int abort = 0;
	int never_timeout = 0;
	int err;

	if (IS_ENABLED(CONFIG_AUTOBOOT_STOP_STR_ENABLE) && !crypt_env_str)
		crypt_env_str = AUTOBOOT_STOP_STR_CRYPT;

	if (!crypt_env_str)
		return 0;

	/* We expect the stop-string to be newline-terminated */
	do {
		if (tstc()) {
			/* Check for input string overflow */
			if (presskey_len >= sizeof(presskey))
				return 0;

			presskey[presskey_len] = getchar();

			if ((presskey[presskey_len] == '\r') ||
			    (presskey[presskey_len] == '\n')) {
				if (IS_ENABLED(CONFIG_AUTOBOOT_NEVER_TIMEOUT) &&
				    !presskey_len) {
					never_timeout = 1;
					continue;
				}
				presskey[presskey_len] = '\0';
				err = crypt_compare(crypt_env_str, presskey,
						    &abort);
				if (err)
					debug_bootkeys(
						"crypt_compare() failed with: %s\n",
						errno_str(err));
				/* you had one chance */
				break;
			} else {
				presskey_len++;
			}
		}
		udelay(10000);
	} while (never_timeout || get_ticks() <= etime);

	return abort;
}

/*
 * Use a "constant-length" time compare function for this
 * hash compare:
 *
 * https://crackstation.net/hashing-security.htm
 */
static int slow_equals(u8 *a, u8 *b, int len)
{
	int diff = 0;
	int i;

	for (i = 0; i < len; i++)
		diff |= a[i] ^ b[i];

	return diff == 0;
}

/**
 * passwd_abort_sha256() - check for a hashed key sequence to abort booting
 *
 * This checks for the user entering a SHA256 hash within a given time.
 *
 * @etime: Timeout value ticks (stop when get_ticks() reachs this)
 * Return: 0 if autoboot should continue, 1 if it should stop
 */
static int passwd_abort_sha256(uint64_t etime)
{
	const char *sha_env_str = env_get("bootstopkeysha256");
	u8 sha_env[SHA256_SUM_LEN];
	u8 *sha;
	char *presskey;
	char *c;
	const char *algo_name = "sha256";
	u_int presskey_len = 0;
	int abort = 0;
	int size = sizeof(sha);
	int ret;

	if (sha_env_str == NULL)
		sha_env_str = AUTOBOOT_STOP_STR_SHA256;

	presskey = malloc_cache_aligned(DELAY_STOP_STR_MAX_LENGTH);
	c = strstr(sha_env_str, ":");
	if (c && (c - sha_env_str < DELAY_STOP_STR_MAX_LENGTH)) {
		/* preload presskey with salt */
		memcpy(presskey, sha_env_str, c - sha_env_str);
		presskey_len = c - sha_env_str;
		sha_env_str = c + 1;
	}
	/*
	 * Generate the binary value from the environment hash value
	 * so that we can compare this value with the computed hash
	 * from the user input
	 */
	ret = hash_parse_string(algo_name, sha_env_str, sha_env);
	if (ret) {
		printf("Hash %s not supported!\n", algo_name);
		return 0;
	}

	sha = malloc_cache_aligned(SHA256_SUM_LEN);
	size = SHA256_SUM_LEN;
	/*
	 * We don't know how long the stop-string is, so we need to
	 * generate the sha256 hash upon each input character and
	 * compare the value with the one saved in the environment
	 */
	do {
		if (tstc()) {
			/* Check for input string overflow */
			if (presskey_len >= DELAY_STOP_STR_MAX_LENGTH) {
				free(presskey);
				free(sha);
				return 0;
			}

			presskey[presskey_len++] = getchar();

			/* Calculate sha256 upon each new char */
			hash_block(algo_name, (const void *)presskey,
				   presskey_len, sha, &size);

			/* And check if sha matches saved value in env */
			if (slow_equals(sha, sha_env, SHA256_SUM_LEN))
				abort = 1;
		}
		udelay(10000);
	} while (!abort && get_ticks() <= etime);

	free(presskey);
	free(sha);
	return abort;
}

/**
 * passwd_abort_key() - check for a key sequence to aborted booting
 *
 * This checks for the user entering a string within a given time.
 *
 * @etime: Timeout value ticks (stop when get_ticks() reachs this)
 * Return: 0 if autoboot should continue, 1 if it should stop
 */
static int passwd_abort_key(uint64_t etime)
{
	int abort = 0;
	struct {
		char *str;
		u_int len;
		int retry;
	}
	delaykey[] = {
		{ .str = env_get("bootdelaykey"),  .retry = 1 },
		{ .str = env_get("bootstopkey"),   .retry = 0 },
	};

	char presskey[DELAY_STOP_STR_MAX_LENGTH];
	int presskey_len = 0;
	int presskey_max = 0;
	int i;

#  ifdef CONFIG_AUTOBOOT_DELAY_STR
	if (delaykey[0].str == NULL)
		delaykey[0].str = CONFIG_AUTOBOOT_DELAY_STR;
#  endif
#  ifdef CONFIG_AUTOBOOT_STOP_STR
	if (delaykey[1].str == NULL)
		delaykey[1].str = CONFIG_AUTOBOOT_STOP_STR;
#  endif

	for (i = 0; i < sizeof(delaykey) / sizeof(delaykey[0]); i++) {
		delaykey[i].len = delaykey[i].str == NULL ?
				    0 : strlen(delaykey[i].str);
		delaykey[i].len = delaykey[i].len > DELAY_STOP_STR_MAX_LENGTH ?
				    DELAY_STOP_STR_MAX_LENGTH : delaykey[i].len;

		presskey_max = presskey_max > delaykey[i].len ?
				    presskey_max : delaykey[i].len;

		debug_bootkeys("%s key:<%s>\n",
			       delaykey[i].retry ? "delay" : "stop",
			       delaykey[i].str ? delaykey[i].str : "NULL");
	}

	/* In order to keep up with incoming data, check timeout only
	 * when catch up.
	 */
	do {
		if (tstc()) {
			if (presskey_len < presskey_max) {
				presskey[presskey_len++] = getchar();
			} else {
				for (i = 0; i < presskey_max - 1; i++)
					presskey[i] = presskey[i + 1];

				presskey[i] = getchar();
			}
		}

		for (i = 0; i < sizeof(delaykey) / sizeof(delaykey[0]); i++) {
			if (delaykey[i].len > 0 &&
			    presskey_len >= delaykey[i].len &&
				memcmp(presskey + presskey_len -
					delaykey[i].len, delaykey[i].str,
					delaykey[i].len) == 0) {
					debug_bootkeys("got %skey\n",
						delaykey[i].retry ? "delay" :
						"stop");

				/* don't retry auto boot */
				if (!delaykey[i].retry)
					bootretry_dont_retry();
				abort = 1;
			}
		}
		udelay(10000);
	} while (!abort && get_ticks() <= etime);

	return abort;
}

/**
 * flush_stdin() - drops all pending characters from stdin
 */
static void flush_stdin(void)
{
	while (tstc())
		(void)getchar();
}

/**
 * fallback_to_sha256() - check whether we should fall back to sha256
 *                        password checking
 *
 * This checks for the environment variable `bootstopusesha256` in case
 * sha256-fallback has been enabled via the config setting
 * `AUTOBOOT_SHA256_FALLBACK`.
 *
 * Return: `false` if we must not fall-back, `true` if plain sha256 should be tried
 */
static bool fallback_to_sha256(void)
{
	if (IS_ENABLED(CONFIG_AUTOBOOT_SHA256_FALLBACK))
		return env_get_yesno("bootstopusesha256") == 1;
	else if (IS_ENABLED(CONFIG_CRYPT_PW))
		return false;
	else
		return true;
}

/***************************************************************************
 * Watch for 'delay' seconds for autoboot stop or autoboot delay string.
 * returns: 0 -  no key string, allow autoboot 1 - got key string, abort
 */
static int abortboot_key_sequence(int bootdelay)
{
	int abort;
	uint64_t etime = endtick(bootdelay);

	if (IS_ENABLED(CONFIG_AUTOBOOT_FLUSH_STDIN))
		flush_stdin();
#  ifdef CONFIG_AUTOBOOT_PROMPT
	/*
	 * CONFIG_AUTOBOOT_PROMPT includes the %d for all boards.
	 * To print the bootdelay value upon bootup.
	 */
	printf(CONFIG_AUTOBOOT_PROMPT, bootdelay);
#  endif

	if (IS_ENABLED(CONFIG_AUTOBOOT_ENCRYPTION)) {
		if (IS_ENABLED(CONFIG_CRYPT_PW) && !fallback_to_sha256())
			abort = passwd_abort_crypt(etime);
		else
			abort = passwd_abort_sha256(etime);
	} else {
		abort = passwd_abort_key(etime);
	}
	if (!abort)
		debug_bootkeys("key timeout\n");

	return abort;
}

#ifdef CONFIG_BOOT_ESWIN_VPU7702
static int abortboot_single_key(int bootdelay)
{
	int abort = 0;
	unsigned long ts;
	u32 testreg_var = 0;

	if (save_board_info_to_fdt()) {
		printf("Failed to save board info to FDT\n");
	}

	/*
	 * set test reg to info host ready for loading image
	 */
	writel(READY_SIGN, (u32 *)TEST_REG0);
	testreg_var = readl((u32 *)TEST_REG0);
	if (testreg_var != READY_SIGN) {
		printf("WARNING! set test reg failed. value is %d\n", testreg_var);
	}
	bootdelay = 5;
#ifdef CONFIG_CMD_ESWIN_DIE
	run_command_list("eswin_die", -1, 0);
#endif
	printf("Hit any key to cmd line, or will autoboot once image is loaded.\n");

	/*
	 * Check if key already pressed
	 */
	if (tstc()) {	/* we got a key press	*/
		getchar();	/* consume input	*/
		puts("\b\b\b 0");
		abort = 1;	/* don't auto boot	*/
	}

	while ((bootdelay > 0) && (!abort)) {
		//--bootdelay;
		/* delay 1000 ms */
		ts = get_timer(0);
		do {
			if (tstc()) {	/* we got a key press	*/
				int key;

				abort  = 1;	/* don't auto boot	*/
				bootdelay = 0;	/* no more delay	*/
				key = getchar();/* consume input	*/
				if (IS_ENABLED(CONFIG_AUTOBOOT_USE_MENUKEY))
					menukey = key;
				break;
			}

			/*
			 * autoboot if OS image is loaded from host
			 * by checking test reg
			 */
			testreg_var = readl((u32 *)TEST_REG0);
			debug_bootkeys("testreg value is %x.\n", testreg_var);
			if (testreg_var == BOOT_SIGN) {
				printf("OS image is loaded. Now autobooting ... \n");
				invalidate_dcache_range(0x140000000, 0x180000000);
				run_command_list("bootm 0x140000000", -1, 0);
			}

			udelay(10000);
		} while (!abort && get_timer(ts) < 1000);

		//printf("\b\b\b%2d ", bootdelay);
	}

	putc('\n');

	return abort;
}
#else
static int abortboot_single_key(int bootdelay)
{
	int abort = 0;
	unsigned long ts;

	printf("Hit any key to stop autoboot: %2d ", bootdelay);

	/*
	 * Check if key already pressed
	 */
	if (tstc()) {	/* we got a key press	*/
		getchar();	/* consume input	*/
		puts("\b\b\b 0");
		abort = 1;	/* don't auto boot	*/
	}

	while ((bootdelay > 0) && (!abort)) {
		--bootdelay;
		/* delay 1000 ms */
		ts = get_timer(0);
		do {
			if (tstc()) {	/* we got a key press	*/
				int key;

				abort  = 1;	/* don't auto boot	*/
				bootdelay = 0;	/* no more delay	*/
				key = getchar();/* consume input	*/
				if (IS_ENABLED(CONFIG_AUTOBOOT_USE_MENUKEY))
					menukey = key;
				break;
			}
			udelay(10000);
		} while (!abort && get_timer(ts) < 1000);

		printf("\b\b\b%2d ", bootdelay);
	}

	putc('\n');

	return abort;
}
#endif

static int abortboot(int bootdelay)
{
	int abort = 0;

	if (bootdelay >= 0) {
		if (autoboot_keyed())
			abort = abortboot_key_sequence(bootdelay);
		else
			abort = abortboot_single_key(bootdelay);
	}

	if (IS_ENABLED(CONFIG_SILENT_CONSOLE) && abort)
		gd->flags &= ~GD_FLG_SILENT;

	return abort;
}

static void process_fdt_options(void)
{
#ifdef CONFIG_TEXT_BASE
	ulong addr;

	/* Add an env variable to point to a kernel payload, if available */
	addr = ofnode_conf_read_int("kernel-offset", 0);
	if (addr)
		env_set_addr("kernaddr", (void *)(CONFIG_TEXT_BASE + addr));

	/* Add an env variable to point to a root disk, if available */
	addr = ofnode_conf_read_int("rootdisk-offset", 0);
	if (addr)
		env_set_addr("rootaddr", (void *)(CONFIG_TEXT_BASE + addr));
#endif /* CONFIG_TEXT_BASE */
}

const char *bootdelay_process(void)
{
	char *s;
	int bootdelay;

	bootcount_inc();

	s = env_get("bootdelay");
	bootdelay = s ? (int)simple_strtol(s, NULL, 10) : CONFIG_BOOTDELAY;

	/*
	 * Does it really make sense that the devicetree overrides the user
	 * setting? It is possibly helpful for security since the device tree
	 * may be signed whereas the environment is often loaded from storage.
	 */
	if (IS_ENABLED(CONFIG_OF_CONTROL))
		bootdelay = ofnode_conf_read_int("bootdelay", bootdelay);

	debug("### main_loop entered: bootdelay=%d\n\n", bootdelay);

	if (IS_ENABLED(CONFIG_AUTOBOOT_MENU_SHOW))
		bootdelay = menu_show(bootdelay);
	bootretry_init_cmd_timeout();

#ifdef CONFIG_POST
	if (gd->flags & GD_FLG_POSTFAIL) {
		s = env_get("failbootcmd");
	} else
#endif /* CONFIG_POST */
	if (bootcount_error())
		s = env_get("altbootcmd");
	else
		s = env_get("bootcmd");

	if (IS_ENABLED(CONFIG_OF_CONTROL))
		process_fdt_options();
	stored_bootdelay = bootdelay;

	return s;
}

void autoboot_command(const char *s)
{
	debug("### main_loop: bootcmd=\"%s\"\n", s ? s : "<UNDEFINED>");

	if (s && (stored_bootdelay == -2 ||
		 (stored_bootdelay != -1 && !abortboot(stored_bootdelay)))) {
		bool lock;
		int prev;

		lock = autoboot_keyed() &&
			!IS_ENABLED(CONFIG_AUTOBOOT_KEYED_CTRLC);
		if (lock)
			prev = disable_ctrlc(1); /* disable Ctrl-C checking */

		run_command_list(s, -1, 0);

		if (lock)
			disable_ctrlc(prev);	/* restore Ctrl-C checking */
	} else {
		env_set("stdout", "vidconsole,serial");
		run_command("cls", 0);
	}

	if (IS_ENABLED(CONFIG_AUTOBOOT_USE_MENUKEY) &&
	    menukey == AUTOBOOT_MENUKEY) {
		s = env_get("menucmd");
		if (s)
			run_command_list(s, -1, 0);
	}
}
