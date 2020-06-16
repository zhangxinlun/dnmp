/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.fastken.com/ for more detail.
**/

#ifndef SHARED_FUNC_H
#define SHARED_FUNC_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "common_define.h"
#include "ini_file_reader.h"

#ifdef __cplusplus
extern "C" {
#endif

/** lowercase the string
 *  parameters:
 *  	src: input string, will be changed
 *  return: lowercased string
*/
char *toLowercase(char *src);

/** uppercase the string
 *  parameters:
 *  	src: input string, will be changed
 *  return: uppercased string
*/
char *toUppercase(char *src);


/** date format to string
 *  parameters:
 *  	nTime: unix timestamp
 *  	szDateFormat: date format, more detail man strftime
 *  	buff: store the formated result, can be NULL
 *  	buff_size: buffer size, max bytes can contain
 *  return: formated date string
*/
char *formatDatetime(const time_t nTime, \
	const char *szDateFormat, \
	char *buff, const int buff_size);

/** get character count, only support GB charset
 *  parameters:
 *  	s: the string
 *  return: character count
*/
int getCharLen(const char *s);

/** replace \r and \n to space
 *  parameters:
 *  	s: the string
 *  return: replaced string
*/
char *replaceCRLF2Space(char *s);

/** get the filename absolute path
 *  parameters:
 *  	fileame: the filename
 *  	szAbsPath: store the absolute path
 *  	pathSize: max bytes to contain
 *  return: absolute path, NULL for fail
*/
char *getAbsolutePath(const char *fileame, char *szAbsPath, \
				const int pathSize);

/** get the executable file absolute filename
 *  parameters:
 *  	exeFilename: the executable filename
 *  	szAbsFilename: store the absolute filename
 *  	maxSize: max bytes to contain
 *  return: absolute filename, NULL for fail
*/
char *getExeAbsoluteFilename(const char *exeFilename, char *szAbsFilename, \
		const int maxSize);

#ifndef WIN32

/** get running process count by program name such as fdfs_trackerd
 *  parameters:
 *  	progName: the program name
 * 	bAllOwners: false for only get my proccess count
 *  return: proccess count, >= 0 success, < 0 fail
*/
int getProccessCount(const char *progName, const bool bAllOwners);

/** get running process ids by program name such as fdfs_trackerd
 *  parameters:
 *  	progName: the program name
 * 	bAllOwners: false for only get my proccess count
 * 	pids: store the pids
 * 	arrSize: max pids
 *  return: proccess count, >= 0 success, < 0 fail
*/
int getUserProcIds(const char *progName, const bool bAllOwners, \
			int pids[], const int arrSize);

/** execute program, get it's output
 *  parameters:
 *  	command: the program
 * 	output: store ouput result
 * 	buff_size: output max size (bytes)
 *  return: error no, 0 success, != 0 fail
*/
int getExecResult(const char *command, char *output, const int buff_size);

#endif

/** daemon init
 *  parameters:
 *  	bCloseFiles: if close the stdin, stdout and stderr
 *  return: none
*/
void daemon_init(bool bCloseFiles);

/** convert buffer content to hex string such as 0B82A1
 *  parameters:
 *  	s: the buffer
 *  	len: the buffer length
 *  	szHexBuff: store the hex string (must have enough space)
 *  return: hex string (szHexBuff)
*/
char *bin2hex(const char *s, const int len, char *szHexBuff);

/** parse hex string to binary content
 *  parameters:
 *  	s: the hex string such as 8B04CD
 *  	szBinBuff: store the converted binary content(must have enough space)
 *  	nDestLen: store the converted content length
 *  return: converted binary content (szBinBuff)
*/
char *hex2bin(const char *s, char *szBinBuff, int *nDestLen);

/** print binary buffer as hex string
 *  parameters:
 *  	s: the buffer
 *  	len: the buffer length
 *  return: none
*/
void printBuffHex(const char *s, const int len);

/** 16 bits int convert to buffer (big-endian)
 *  parameters:
 *  	n: 16 bits int value
 *  	buff: the buffer, at least 2 bytes space, no tail \0
 *  return: none
*/
void short2buff(const short n, char *buff);

/** buffer convert to 16 bits int
 *  parameters:
 *  	buff: big-endian 2 bytes buffer
 *  return: 16 bits int value
*/
short buff2short(const char *buff);

/** 32 bits int convert to buffer (big-endian)
 *  parameters:
 *  	n: 32 bits int value
 *  	buff: the buffer, at least 4 bytes space, no tail \0
 *  return: none
*/
void int2buff(const int n, char *buff);

/** buffer convert to 32 bits int
 *  parameters:
 *  	buff: big-endian 4 bytes buffer
 *  return: 32 bits int value
*/
int buff2int(const char *buff);

/** long (64 bits) convert to buffer (big-endian)
 *  parameters:
 *  	n: 64 bits int value
 *  	buff: the buffer, at least 8 bytes space, no tail \0
 *  return: none
*/
void long2buff(int64_t n, char *buff);

/** buffer convert to 64 bits int
 *  parameters:
 *  	buff: big-endian 8 bytes buffer
 *  return: 64 bits int value
*/
int64_t buff2long(const char *buff);


/** 32 bits float convert to buffer (big-endian)
 *  parameters:
 *  	n: 32 bits float value
 *  	buff: the buffer, at least 4 bytes space, no tail \0
 *  return: none
*/
static inline void float2buff(float f, char *buff)
{
    int *p;
    p = (int *)&f;
    int2buff(*p, buff);
}

/** buffer convert to 32 bits float
 *  parameters:
 *  	buff: big-endian 8 bytes buffer
 *  return: 32 bits float value
*/
static inline float buff2float(const char *buff)
{
    int n;
    float *p;

    n = buff2int(buff);
    p = (float *)&n;
    return *p;
}

/** double (64 bits) convert to buffer (big-endian)
 *  parameters:
 *  	n: 64 bits double value
 *  	buff: the buffer, at least 8 bytes space, no tail \0
 *  return: none
*/
static inline void double2buff(double d, char *buff)
{
    int64_t *p;
    p = (int64_t *)&d;
    long2buff(*p, buff);
}

/** buffer convert to 64 bits double
 *  parameters:
 *  	buff: big-endian 8 bytes buffer
 *  return: 64 bits double value
*/
static inline double buff2double(const char *buff)
{
    int64_t n;
    double *p;

    n = buff2long(buff);
    p = (double *)&n;
    return *p;
}

/** trim leading spaces ( \t\r\n)
 *  parameters:
 *  	pStr: the string to trim
 *  return: trimed string porinter as pStr
*/
char *trim_left(char *pStr);

/** trim tail spaces ( \t\r\n)
 *  parameters:
 *  	pStr: the string to trim
 *  return: trimed string porinter as pStr
*/
char *trim_right(char *pStr);

/** trim leading and tail spaces ( \t\r\n)
 *  parameters:
 *  	pStr: the string to trim
 *  return: trimed string porinter as pStr
*/
char *trim(char *pStr);

/** trim leading and tail spaces ( \t\r\n)
 *  parameters:
 *  	pStr: the string to trim
 *  return: trimed string porinter as pStr
*/
static inline char *fc_trim(char *pStr)
{
	trim_right(pStr);
	trim_left(pStr);
	return pStr;
}

/** trim leading spaces ( \t\r\n)
 *  parameters:
 *  	s: the string to trim
 *  return: none
*/
void string_ltrim(string_t *s);

/** trim tail spaces ( \t\r\n)
 *  parameters:
 *  	s: the string to trim
 *  return: none
*/
void string_rtrim(string_t *s);

#define FC_STRING_TRIM(s)  \
    do {  \
        string_ltrim(s);  \
        string_rtrim(s);  \
    } while (0)

static inline void string_trim(string_t *s)
{
    FC_STRING_TRIM(s);
}

/** copy string to BufferInfo
 *  parameters:
 *  	pBuff: the dest buffer
 *  	str: source string
 *  return: error no, 0 success, != 0 fail
*/
int buffer_strcpy(BufferInfo *pBuff, const char *str);

/** copy binary buffer to BufferInfo
 *  parameters:
 *  	pBuff: the dest buffer
 *  	buff: source buffer
 *  	len: source buffer length
 *  return: error no, 0 success, != 0 fail
*/
int buffer_memcpy(BufferInfo *pBuff, const char *buff, const int len);

/** url encode
 *  parameters:
 *  	src: the source string to encode
 *  	src_len: source string length
 *  	dest: store dest string
 *  	dest_len: store the dest string length
 *  return: error no, 0 success, != 0 fail
*/
char *urlencode(const char *src, const int src_len, char *dest, int *dest_len);

/** url decode, terminated with \0
 *  parameters:
 *  	src: the source string to decode
 *  	src_len: source string length
 *  	dest: store dest string
 *  	dest_len: store the dest string length
 *  return: error no, 0 success, != 0 fail
*/
char *urldecode(const char *src, const int src_len, char *dest, int *dest_len);

/** url decode, no terminate with \0
 *  parameters:
 *  	src: the source string to decode
 *  	src_len: source string length
 *  	dest: store dest string
 *  	dest_len: store the dest string length
 *  return: error no, 0 success, != 0 fail
*/
char *urldecode_ex(const char *src, const int src_len, char *dest, int *dest_len);

/** get char occurs count
 *  parameters:
 *  	src: the source string
 *  	seperator: find this char occurs times
 *  return: char occurs count
*/
int getOccurCount(const char *src, const char seperator);


/** get the file line count
 *  parameters:
 *  	filename: the filename
 *  	until_offset: util the file offset, -1 for file end
 *  	line_count: store the line count
 *  return: error no, 0 success, != 0 fail
*/
int fc_get_file_line_count_ex(const char *filename,
        const int64_t until_offset, int64_t *line_count);

static inline int fc_get_file_line_count(const char *filename,
        int64_t *line_count)
{
    const int64_t until_offset = -1;
    return fc_get_file_line_count_ex(filename, until_offset, line_count);
}

/** split string
 *  parameters:
 *  	src: the source string, will be modified by this function
 *  	seperator: seperator char
 *  	nMaxCols: max columns (max split count)
 *  	nColCount: store the columns (array elements) count
 *  return: string array, should call freeSplit to free, return NULL when fail
*/
char **split(char *src, const char seperator, const int nMaxCols, \
		int *nColCount);

/** free split results
 *  parameters:
 *  	p: return by function split
 *  return: none
*/
void freeSplit(char **p);


/** split string
 *  parameters:
 *  	src: the source string, will be modified by this function
 *  	seperator: seperator char
 *  	pCols: store split strings
 *  	nMaxCols: max columns (max split count)
 *  return: string array / column count
*/
int splitEx(char *src, const char seperator, char **pCols, const int nMaxCols);


/** split string
 *  parameters:
 *  	src: the source string
 *  	seperator: seperator char
 *  	dest: store split strings
 *  	max_count: max split count
 *      ignore_empty: if ignore empty string
 *  return: string array count
*/
int split_string_ex(const string_t *src, const char seperator,
        string_t *dest, const int max_count, const bool ignore_empty);

/** split string by delimiter characters
 *  parameters:
 *  	src: the source string, will be modified by this function
 *  	delim: the delimiter characters
 *  	pCols: store split strings
 *  	nMaxCols: max columns (max split count)
 *  return: string array / column count
*/
int fc_split_string(char *src, const char *delim, char **pCols, const int nMaxCols);

/** if the input string contains all delimiter characters
 *  parameters:
 *  	str: the input string
 *  	delim: the delimiter characters
 *  return: true for contains all delimiter characters, otherwise false
*/
bool fc_match_delim(const char *str, const char *delim);

/** split string
 *  parameters:
 *  	src: the source string, will be modified by this function
 *  	seperator: seperator char
 *  	pCols: store split strings
 *  	nMaxCols: max columns (max split count)
 *  return: string array / column count
*/
int my_strtok(char *src, const char *delim, char **pCols, const int nMaxCols);

/** check file exist
 *  parameters:
 *  	filename: the filename
 *  return: true if file exists, otherwise false
*/
bool fileExists(const char *filename);

/** check if a directory
 *  parameters:
 *  	filename: the filename
 *  return: true for directory
*/
bool isDir(const char *filename);

/** check if a regular file
 *  parameters:
 *  	filename: the filename
 *  return: true for regular file
*/
bool isFile(const char *filename);

/** check if filename securty, /../ ocur in filename not allowed
 *  parameters:
 *  	filename: the filename
 *  	len: filename length
 *  return: true for regular file
*/
bool is_filename_secure(const char *filename, const int len);

/** load log_level from config context
 *  parameters:
 *  	pIniContext: the config context
 *  return: none
*/
void load_log_level(IniContext *pIniContext);

/** load log_level from config file
 *  parameters:
 *  	conf_filename: the config filename
 *  return: none
*/
int load_log_level_ex(const char *conf_filename);

/** set global log level
 *  parameters:
 *  	pLogLevel: log level string value
 *  return: none
*/
void set_log_level(char *pLogLevel);

/** load allow hosts from config context
 *  parameters:
 *  	pIniContext: the config context
 *  	allow_ip_addrs: store allow ip addresses
 *  	allow_ip_count: store allow ip address count
 *  return: error no , 0 success, != 0 fail
*/
int load_allow_hosts(IniContext *pIniContext, \
		in_addr_t **allow_ip_addrs, int *allow_ip_count);

/** get time item from config context
 *  parameters:
 *  	pIniContext: the config context
 *  	item_name: item name in config file, time format as hour:minute, such as 15:25
 *  	pTimeInfo: store time info
 *  	default_hour: default hour value
 *  	default_minute: default minute value
 *  return: error no , 0 success, != 0 fail
*/
int get_time_item_from_conf(IniContext *pIniContext, \
		const char *item_name, TimeInfo *pTimeInfo, \
		const byte default_hour, const byte default_minute);


/** get time item from string
 *  parameters:
 *  	pValue: the time string, format as hour:minute, such as 15:25
 *  	item_name: item name in config file
 *  	pTimeInfo: store time info
 *  	default_hour: default hour value
 *  	default_minute: default minute value
 *  return: error no , 0 success, != 0 fail
*/
int get_time_item_from_str(const char *pValue, const char *item_name,
        TimeInfo *pTimeInfo, const byte default_hour,
        const byte default_minute);

/** trim path tail char /
 *  parameters:
 *  	filePath: the file path to chop
 *  return: none
*/
void chopPath(char *filePath);

/** get file content
 *  parameters:
 *  	filename: the filename
 *  	buff: return the buff, must be freed
 *  	file_size: store the file size
 *  return: error no , 0 success, != 0 fail
*/
int getFileContent(const char *filename, char **buff, int64_t *file_size);

/** get file content
 *  parameters:
 *  	filename: the filename
 *  	buff: the buff to store file content
 *      offset: the start offset
 *  	size: specify the size to fetch and return the fetched size
 *  return: error no , 0 success, != 0 fail
*/
int getFileContentEx(const char *filename, char *buff, \
		int64_t offset, int64_t *size);

/** get file size
 *  parameters:
 *  	filename: the filename
 *  	file_size: store the file size
 *  return: error no , 0 success, != 0 fail
*/
int getFileSize(const char *filename, int64_t *file_size);

/** write to file
 *  parameters:
 *  	filename: the filename to write
 *  	buff: the buffer to write
 *  	file_size: the file size
 *  return: error no , 0 success, != 0 fail
*/
int writeToFile(const char *filename, const char *buff, const int file_size);

/** safe write to file, first write to tmp file, then rename to true filename
 *  parameters:
 *  	filename: the filename to write
 *  	buff: the buffer to write
 *  	file_size: the file size
 *  return: error no , 0 success, != 0 fail
*/
int safeWriteToFile(const char *filename, const char *buff, \
		const int file_size);

/** get a line from file
 *  parameters:
 *  	fd: the fd to read
 *  	buff: the buffer to store the line
 *  	size: the buffer max size
 *  	once_bytes: the bytes per read
 *  return: error no , 0 success, != 0 fail
*/
int fd_gets(int fd, char *buff, const int size, int once_bytes);

/** set unix rlimit
 *  parameters:
 *  	resource: resource id, please see sys/resource.h
 *  	value: the value to set
 *  return: error no , 0 success, != 0 fail
*/
int set_rlimit(int resource, const rlim_t value);

/** fcntl add flags such as O_NONBLOCK or FD_CLOEXEC
 *  parameters:
 *  	fd: the fd to set
 *  	get_cmd: the get command
 *  	set_cmd: the set command
 *  	adding_flags: the flags to add
 *  return: error no , 0 success, != 0 fail
*/
int fcntl_add_flags(int fd, int get_cmd, int set_cmd, int adding_flags);

/** set fd flags such as O_NONBLOCK
 *  parameters:
 *  	fd: the fd to set
 *  	adding_flags: the flags to add
 *  return: error no , 0 success, != 0 fail
*/
int fd_add_flags(int fd, int adding_flags);

/** set non block mode
 *  parameters:
 *  	fd: the fd to set
 *  return: error no , 0 success, != 0 fail
*/
#define set_nonblock(fd) fd_add_flags(fd, O_NONBLOCK)

/** set fd FD_CLOEXEC flags
 *  parameters:
 *  	fd: the fd to set
 *  return: error no , 0 success, != 0 fail
*/
int fd_set_cloexec(int fd);

/** set run by group and user
 *  parameters:
 *  	group_name: the group name, can be NULL or empty
 *  	username: the username, can be NULL or empty
 *  return: error no , 0 success, != 0 fail
*/
int set_run_by(const char *group_name, const char *username);

/** compare ip address, type is (in_addr_t *)
 *  parameters:
 *  	p1: the first ip address
 *  	p2: the second ip address
 *  return: > 0 when p1 > p2, 0 when p1 == p2, < 0 when p1 < p2
*/
int cmp_by_ip_addr_t(const void *p1, const void *p2);

/** parse bytes
 *  parameters:
 *  	pStr: the string to parse
 *  	default_unit_bytes: default unit if not specified the unit like MB etc.
 *  	bytes: store the parsed bytes
 *  return: error no , 0 success, != 0 fail
*/
int parse_bytes(char *pStr, const int default_unit_bytes, int64_t *bytes);

/** set rand seed
 *  return: error no , 0 success, != 0 fail
*/
int set_rand_seed();

/** set timer wrapper
 *  parameters:
 *  	first_remain_seconds: remain time for first time, in seconds
 *  	interval: the interval
 *  	sighandler: handler function
 *  return: error no , 0 success, != 0 fail
*/
int set_timer(const int first_remain_seconds, const int interval, \
		void (*sighandler)(int));

/** set file access and modified times
 *  parameters:
 *  	filename: the file to modify times
 *  	new_time: the time to set
 *  return: error no , 0 success, != 0 fail
*/
int set_file_utimes(const char *filename, const time_t new_time);

/** ignore singal pipe (SIGPIPE)
 *  return: error no , 0 success, != 0 fail
*/
int ignore_signal_pipe();

double get_line_distance_km(const double lat1, const double lon1,
        const double lat2, const double lon2);

/** is private ip address for IPv4
 *  return: true for private ip, otherwise false
 */
bool is_private_ip(const char* ip);


/** get current time in us
 *  return: current time
 */
int64_t get_current_time_us();

#define get_current_time_ms() (get_current_time_us() / 1000)

/** is the number power 2
 *  parameters:
 *     n: the number to test
 *  return: true for power 2, otherwise false
*/
bool is_power2(const int64_t n);

/** set file read lock
 *  parameters:
 *  	fd: the file descriptor to lock
 *  return: error no, 0 for success, != 0 fail
*/
int file_read_lock(int fd);

/** set file write lock
 *  parameters:
 *  	fd: the file descriptor to lock
 *  return: error no, 0 for success, != 0 fail
*/
int file_write_lock(int fd);

/** file unlock
 *  parameters:
 *  	fd: the file descriptor to unlock
 *  return: error no, 0 for success, != 0 fail
*/
int file_unlock(int fd);

/** try file read lock
 *  parameters:
 *  	fd: the file descriptor to lock
 *  return: error no, 0 for success, != 0 fail
*/
int file_try_read_lock(int fd);

/** try file write lock
 *  parameters:
 *  	fd: the file descriptor to lock
 *  return: error no, 0 for success, != 0 fail
*/
int file_try_write_lock(int fd);

/** try file unlock
 *  parameters:
 *  	fd: the file descriptor to unlock
 *  return: error no, 0 for success, != 0 fail
*/
int file_try_unlock(int fd);

/** is a leading spaces line
 *  parameters:
 *  	content: the whole string content
 *  	current: the current line
 *  return: error no, 0 for success, != 0 fail
*/
bool isLeadingSpacesLine(const char *content, const char *current);

/** is a trailing spaces line
 *  parameters:
 *  	tail: the current line tail
 *  	end: the string end ptr
 *  return: error no, 0 for success, != 0 fail
*/
bool isTrailingSpacesLine(const char *tail, const char *end);

/** write to file
 *  parameters:
 *  	fd: the fd to write
 *  	buf: the buffer
 *  	nbyte: the buffer length
 *  return: written bytes for success, -1 when fail
*/
ssize_t fc_safe_write(int fd, const char *buf, const size_t nbyte);

/** lock and write to file
 *  parameters:
 *  	fd: the fd to write
 *  	buf: the buffer
 *  	nbyte: the buffer length
 *  return: written bytes for success, -1 when fail
*/
ssize_t fc_lock_write(int fd, const char *buf, const size_t nbyte);

/** read from file
 *  parameters:
 *  	fd: the fd to read
 *  	buf: the buffer
 *  	count: expect read bytes
 *  return: read bytes for success, -1 when fail
*/
ssize_t fc_safe_read(int fd, char *buf, const size_t count);

/** ftok with hash code
 *  parameters:
 *  	path: the file path
 *  	proj_id: the project id
 *  return: read bytes for success, -1 when fail
*/
key_t fc_ftok(const char *path, const int proj_id);

/** convert int to string
 *  parameters:
 *  	n: the 32 bits integer
 *      buff: output buffer
 *      thousands_separator: if add thousands separator
 *  return: string buffer
*/
const char *int2str(const int n, char *buff, const bool thousands_separator);

static inline const char *int_to_comma_str(const int n, char *buff)
{
    return int2str(n, buff, true);
}

/** convert long to string
 *  parameters:
 *  	n: the 64 bits integer
 *      buff: output buffer
 *      thousands_separator: if add thousands separator
 *  return: string buffer
*/
const char *long2str(const int64_t n, char *buff, const bool thousands_separator);

static inline const char *long_to_comma_str(const int64_t n, char *buff)
{
    return long2str(n, buff, true);
}

/** if the string starts with the needle string
 *  parameters:
 *  	str: the string to detect
 *      needle: the needle string
 *  return: true for starts with the needle string, otherwise false
*/
bool starts_with(const char *str, const char *needle);

/** if the string ends with the needle string
 *  parameters:
 *  	str: the string to detect
 *      needle: the needle string
 *  return: true for ends with the needle string, otherwise false
*/
bool ends_with(const char *str, const char *needle);

/** strdup
 *  parameters:
 *  	str: the string to duplicate
 *      len: the length of string
 *  return: the duplicated string, NULL for fail
*/
char *fc_strdup(const char *str, const int len);

/** memmem
 *  parameters:
 *  	str: the string to match
 *      needle: the needle string
 *  return: the matched string, NULL for fail
*/
const char *fc_memmem(const string_t *str, const string_t *needle);

/** memmem
 *  parameters:
 *  	str: the string to match
 *      needle: the needle string
 *  return: the matched string, NULL for fail
*/
const char *fc_memrchr(const char *str, const int ch, const int len);

/** format HTTP Date as: Sat, 11 Mar 2017 21:49:51 GMT
 *  parameters:
 *  	t: the time to format
 *      buffer: the buffer
 *  return: formatted GMT date string
*/
char *format_http_date(time_t t, BufferInfo *buffer);

/** return full path for the filename (the second parameter)
 *  parameters:
 *  	from: the input full path filename to get base path
 *      filename: the filename to resolve path
 *      full_filename: store the resolved full path filename
 *      size: the max size of full_filename
 *  return: the resolved full path filename
*/
char *resolve_path(const char *from, const char *filename,
        char *full_filename, const int size);

/** get gzip command full filename
 *  return: the gzip command full filename
*/
const char *get_gzip_command_filename();

/** delete file
 *  parameters:
 *      filename: the filename to delete
 *      caption: the caption of this filename
 *  return: error no, 0 success, != 0 fail
*/
int fc_delete_file_ex(const char *filename, const char *caption);

static inline int fc_delete_file(const char *filename)
{
    return fc_delete_file_ex(filename, "");
}

/** if prime number
 *  parameters:
 *  	n: the number to detect
 *  return: true for prime number, otherwise false
*/
bool fc_is_prime(const int n);


/** find the largest prime number not greater than n
 *  parameters:
 *  	n: the number to detect
 *  return: the largest prime number near n
*/
int fc_floor_prime(const int n);

/** find the smallest prime number not less than n
 *  parameters:
 *  	n: the number to detect
 *  return: the smallest prime number near n
*/
int fc_ceil_prime(const int n);

/** init buffer
 *  parameters:
 *      buffer: the buffer to init
 *      buffer_size: the buffer size
 *  return: error no, 0 success, != 0 fail
*/
int fc_init_buffer(BufferInfo *buffer, const int buffer_size);

/** free buffer
 *  parameters:
 *      buffer: the buffer to free
 *  return: none
*/
void fc_free_buffer(BufferInfo *buffer);


static inline int fc_get_umask()
{
    mode_t mode;

    mode = umask(0); //fetch
    umask(mode);     //restore
    return mode;
}

int fc_check_mkdir_ex(const char *path, const mode_t mode, bool *create);

static inline int fc_check_mkdir(const char *path, const mode_t mode)
{
    bool create;
    return fc_check_mkdir_ex(path, mode, &create);
}

#ifdef __cplusplus
}
#endif

#endif
