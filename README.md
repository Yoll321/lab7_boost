# Duplicate File Finder

A command-line tool for finding duplicate files by scanning directories and comparing file contents block-by-block using CRC32 or MD5 hashing.

## Usage

```bash
./duplicate-finder [options]
```

## Options

| Option | Short | Type | Default | Description |
|---|---|---|---|---|
| `--path` | `-P` | string(s), multitoken | `.` (current directory) | Paths to scan for files. Accepts multiple values, e.g. `--path dir1 dir2 dir3`. |
| `--exclude` | `-E` | string(s), multitoken | — | Paths to exclude from scanning. Accepts multiple values. |
| `--depth` | `-D` | int | `-1` (unlimited) | Scanning depth. `0` disables recursion (top level only); unset/negative means unlimited depth. |
| `--min-size` | — | unsigned integer | `1` | Minimum file size (in bytes) to include in the scan. Files smaller than this are skipped. |
| `--masks` | — | string(s), multitoken | — (all files match) | Wildcard filename masks to filter by, e.g. `*.txt`. Accepts multiple values. |
| `--block-size` | `-S` | unsigned integer | **required** | Block size (in bytes) used when reading and hashing files. |
| `--hash` | `-H` | string | `crc32` | Hashing algorithm to use for comparison. Accepted values: `crc32`, `md5`. |

## Examples

Scan the current directory with a 4 KB block size, using CRC32:

```bash
./duplicate-finder --block-size 4096
```

Scan specific directories, excluding one, with MD5 hashing:

```bash
./duplicate-finder --path ~/Documents ~/Downloads --exclude ~/Downloads/tmp --hash md5 --block-size 8192
```

Scan only `.txt` and `.log` files, at most 2 levels deep, skipping files smaller than 1 KB:

```bash
./duplicate-finder --masks "*.txt" "*.log" --depth 2 --min-size 1024 --block-size 4096
```

Scan without recursing into subdirectories:

```bash
./duplicate-finder --depth 0 --block-size 4096
```

## Notes

- `--block-size` is **required**; the program will print an error and exit if it isn't provided.
- Options that accept multiple values (`--path`, `--exclude`, `--masks`) use `multitoken()`, so values are separated by spaces rather than repeating the flag — though repeating the flag also works and accumulates values.
- If `--hash` is given an unrecognized value, the program falls back to `crc32` and prints a warning.
- Masks support `*` (any sequence of characters) and `?` (any single character), similar to shell glob patterns.
