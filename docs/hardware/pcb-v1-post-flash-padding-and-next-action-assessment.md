# PCB V1.0 Post-Flash Padding and Next-Action Assessment

> **HOST-ONLY POST-ATTEMPT REVIEW**
>
> **NO DEVICE ACCESS**
>
> **NOT AUTHORIZED**
>
> **DO NOT EXECUTE**

## 1. Executive Summary

The one 2026-07-26 App-only write remains
**STOPPED / INCONCLUSIVE — RANGE-SCOPE DEVIATION**. Compatibility remains
**B — PLAUSIBLE BUT NOT PROVEN**, OpenSpec Task 3.4 remains
**NOT COMPLETED**, and device, Flash, startup/observation, and rollback
authorization are all `NONE`.

This host-only review reaches three bounded conclusions:

1. The sector erase envelope already covered
   `0x00020000-0x00047FFF`. In the verified historical backup, all 960 bytes
   at `0x00047440-0x000477FF` and all 2048 bytes at
   `0x00047800-0x00047FFF` were non-`0xFF` bytes belonging to the valid
   historical Xiaozhi App image. The erase therefore intended to replace
   those 3008 historical data bytes with the erased logical value before any
   final-block padding was processed.
2. Installed esptool v4.12.dev3 did send the complete final 1024-byte ROM
   write block: 64 candidate bytes followed by 960 `0xFF` bytes. An erased
   NOR main-array byte is logically `0xFF`; programming `0xFF` requests no
   additional user-data bit transition from `1` to `0`. This supports no
   additional **logical main-array data change** in the padding range beyond
   the preceding sector erase. It does not prove the absence of every
   physical side effect: actual tail readback, internal ECC state, program
   stress, wear, disturb, and neighboring-cell effects remain unverified.
3. The candidate's valid ESP image ends exactly at absolute
   `0x00047440`. The candidate checksum and appended SHA-256 end before the
   960-byte transport padding, and the ESP-IDF image parser derives its
   verification boundary from the segment table, 16-byte checksum boundary,
   and appended-hash flag. In the standard non-Secure-Boot simple-hash path,
   the padding and 2048-byte erased tail are not verified. A Secure Boot v2
   signature path can additionally hash `0xFF` alignment through the next
   4 KiB boundary and then expect a signature block; the preserved
   `v5.5.3-dirty` configuration is unknown. Neither path changes the
   candidate's own parse, checksum, or appended hash bytes.

The selected next-action conclusion is:

**RECOMMEND STARTUP-ONLY AUTHORIZATION REVIEW**

This is a recommendation to review a future, independent authorization, not
an authorization and not an instruction to operate the device. A single
startup reset plus a bounded 60-second read-only observation would add no
intentional main-array or partition write, provides the only direct evidence
that can resolve preserved-bootloader/candidate runtime compatibility, and is
lower-write-risk than immediately erasing and programming the complete
4128768-byte Level 1 rollback image.

The future review must disclose that normal candidate startup configures the
MXIC Octal Flash through register commands. The configured output-driver and
SPI/STR-OPI mode fields are documented as volatile. The IDF path preserves
the logical values of non-volatile BP/TB fields, but the exact physical effect
of issuing a combined register-write command with unchanged non-volatile
fields is not proven. Standard second-stage bootloader initialization also
calls Flash unlock and conditionally clears non-volatile BP protection bits
if they are set. Prior normal boots and the successful App-range erase/write
strongly support that the relevant protection state is already unlocked, but
it was not read in this review. The exact `v5.5.3-dirty` vendor bootloader is
also not source-auditable. These residual risks must be acknowledged in that
future startup-only review. No device action is permitted by this report.

## 2. Scope

This assessment is limited to:

- committed records and Markdown;
- the unchanged candidate, staged candidate copies, Level 1 rollback
  artifacts, and verified 2026-07-10 full backup;
- installed esptool v4.12.dev3 source;
- local ESP-IDF v5.5.4 source, generated candidate configuration, build
  metadata, map, ELF, and object symbols;
- official Macronix MX25UM25645G documentation;
- pure-host parsing, hashing, arithmetic, and source inspection.

No port was enumerated or opened. No serial API was called. No device was
connected, reset, observed, identified, read, written, erased, restored, or
rolled back. No `idf.py`, configure, build, monitor, or device command ran.

After drafting, all 16 existing single-attempt harness tests passed in
pure-host mode. Their synthetic write-path output used fakes; the tests
reported zero real serial-open and port-enumeration calls. Change strict
validation and full-repository strict validation also passed.

## 3. Current Device State

The last reported state, not re-observed in this review, is:

| Item | State |
|---|---|
| First Flash attempt | `STOPPED / INCONCLUSIVE` |
| Compatibility | `B — PLAUSIBLE BUT NOT PROVEN` |
| Task 3.4 | `NOT COMPLETED` |
| Last reported execution state | `Staying in bootloader` / ROM Bootloader |
| Runtime observation | `NOT PERFORMED` |
| Device access authorization | `NONE` |
| Flash authorization | `NONE` |
| Startup/observation authorization | `NONE` |
| Rollback authorization | `NONE` |

The current physical Flash contents were not read back. Statements about
post-attempt physical state are therefore bounded by the recorded ROM
responses, esptool mechanism, and offline historical evidence.

## 4. Attempt Evidence

The sanitized attempt record establishes:

- exactly one harness invocation;
- no outer whole-image retry and no failed-block retry;
- reported erase envelope `0x00020000-0x00047FFF`;
- reported `Wrote 161792 bytes at 0x00020000`;
- reported `Hash of data verified`;
- reported `Staying in bootloader`;
- no startup reset, serial observation, readback, rollback, or later device
  action;
- deletion and closure of the one-time external authorization material.

The attempt stopped because the 161792-byte transmitted range exceeded the
explicit 160832-byte candidate-image range by 960 bytes, even though the
extra bytes stayed inside the disclosed sector erase envelope.

## 5. Candidate Image Range

| Field | Value |
|---|---|
| source | `firmware/build/pcb_v1_first_flash_smoke_test.bin` |
| SHA-256 | `1B72A60DE9C9BB42DE401A58D7772B0525AFBC4DC3D8C85BB550D2D758F99DDC` |
| start | `0x00020000` |
| length | `0x00027440` / 160832 bytes |
| end-exclusive | `0x00047440` |
| last byte | `0x0004743F` |
| image | ESP32-S3 image v1, 6 segments |
| header | DOUT / 80 MHz / 16 MB |
| checksum | `0x98`, valid |
| appended validation hash | `35F99732C55EBAEA00AFE9ECD372D0B0C681E871D175290A763A7455943C50F3`, valid |
| secure version | 0 |

The repository candidate and both staged candidate copies were rehashed and
remained byte-identical in size and SHA-256.

## 6. Actual Transmitted Range

For the no-stub ROM path, `FLASH_WRITE_SIZE` is `0x400` / 1024 bytes.
The 160832-byte file requires 158 blocks:

- blocks 0-156: 157 complete 1024-byte blocks;
- block 157: 64 source bytes plus 960 generated `0xFF` bytes.

| Field | Value |
|---|---|
| transmitted start | `0x00020000` |
| transmitted length | `0x00027800` / 161792 bytes |
| transmitted end-exclusive | `0x00047800` |
| last transmitted byte | `0x000477FF` |
| padding start | `0x00047440` |
| padding length | `0x000003C0` / 960 bytes |
| padding value | all `0xFF` |

A pure-host reconstruction produced the same 158-block, 161792-byte stream.
Its exact-file MD5 is `e92dc2a77c9d2f66e376ef5d3c8371cd`; the padded
stream MD5 is `7ed0917eabc5b63b208c0c720586d765`. They are different,
which is material to the verification-scope finding below.

## 7. Erase Envelope

| Field | Value |
|---|---|
| sector size | `0x1000` / 4096 bytes |
| erase start | `0x00020000` |
| erase end-inclusive | `0x00047FFF` |
| erase end-exclusive | `0x00048000` |
| erase length | `0x00028000` / 163840 bytes / 40 sectors |
| erased but never transmitted | `0x00047800-0x00047FFF` / 2048 bytes |

esptool passed an uncompressed source length of 160832 bytes to ROM Flash
Begin. It separately reported the sector-rounded envelope derived from the
file start/end. The recorded ROM operation reported success for that erase
and write sequence, but the last 3008 bytes were not independently read.

## 8. Historical Bytes in Affected Range

The primary full image was rehashed before extraction:

- file:
  `D:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi\esp-vocat_full_flash_32MB_2026-07-10.bin`;
- size: 33554432 bytes;
- SHA-256:
  `72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001`.

Only aggregate lengths, counts, and hashes are recorded. No slice or raw
hexadecimal content is committed.

| Historical range | Length | All `0xFF` | Non-`0xFF` bytes | SHA-256 |
|---|---:|---|---:|---|
| candidate-corresponding `0x00020000-0x0004743F` | 160832 | no | 160803 | `C9B913F13330C4E3D047EAF9C90715B25ECF535494EA6A8057CA3DB233AD4328` |
| padding-corresponding `0x00047440-0x000477FF` | 960 | no | 960 | `4823FFB7303903B42D1653C1B1F76DF54004658877E57ED6CF5285DB0BB43E0F` |
| erase-only tail `0x00047800-0x00047FFF` | 2048 | no | 2048 | `F3F93D0ED3E703DB10A6680DCA1007A57F5A5B2F7ED7422E480029D17B69CCB1` |
| combined historical tail `0x00047440-0x00047FFF` | 3008 | no | 3008 | `07DABFE87BBE00C4A096743DBDBCA36E0AD3DB0937EFCB3E5035B5241E755E50` |

Offline parsing of the historical complete `ota_0` artifact found a valid
Xiaozhi image through relative end-exclusive `0x0027EA00`, or absolute
end-exclusive `0x0029EA00`. Both the padding-corresponding and erase-only
tail ranges were therefore valid historical Xiaozhi App image bytes, not an
already-erased gap.

The sector erase was the operation that intended to remove those 3008
historical non-`0xFF` bytes. Sending `0xFF` afterward cannot restore their old
values. The candidate range was then programmed with the candidate. No
tail readback was authorized, so actual per-cell completion is not directly
proved.

## 9. esptool Final-Block Padding

The audited installed package is esptool v4.12.dev3 under
`E:\Espressif\tools\python\v5.5.4\venv\Lib\site-packages\esptool`.

| Source | Function/class | Relevant logic | Implication | Limitation |
|---|---|---|---|---|
| `cmds.py:598` | `write_flash()` | reads the complete input file | input starts as exact candidate bytes | later header rewriting is possible for special offsets/options |
| `cmds.py:604`; `util.py:52-57` | `write_flash()` / `pad_to()` | prepares to four-byte alignment with default `0xFF` | no change here because `0x27440` is already four-byte aligned | this is distinct from ROM-block padding |
| `cmds.py:606-614` | `write_flash()` | no-stub path prepends `0xFF` only for an unaligned address | no prefix was added because `0x20000` is sector-aligned | none for this exact address |
| `loader.py:260` | `ESPLoader` | ROM `FLASH_WRITE_SIZE = 0x400` | final ROM block size is 1024 bytes | stub loaders use a different size; this attempt was no-stub |
| `cmds.py:650-681` | `write_flash()` | slices by `FLASH_WRITE_SIZE`, pads final block with `b"\xff"`, calls `flash_block()`, and counts padded length | 64 source bytes became one 1024-byte payload; 960 `0xFF` bytes were actually passed to `flash_block()` | ROM internal page handling is not implemented in this Python |
| `loader.py:1022-1033` | `flash_block()` | packs `len(data)`, sequence, reserved words, and all data into `ESP_FLASH_DATA`; checksum covers all data | final command payload length was 1024 and included all padding | command acceptance is not physical cell readback |
| `cmds.py:734-740` | `write_flash()` | prints `bytes_written`, which includes padded block length | explains the reported 161792 bytes | it is a transport/write count, not source-file size |

Therefore esptool **did actually send** the 960 `0xFF` bytes as part of the
complete final 1024-byte ROM Flash Data command.

## 10. ROM Write-Block Behavior

`loader.py:984-1020` calculates 158 blocks and packs ROM Flash Begin as:

- erase-size argument: 160832;
- block count: 158;
- block size: 1024;
- start offset: `0x00020000`.

`targets/esp32.py:327-328` returns the requested source size as the erase-size
parameter inherited by ESP32-S3. `cmds.py:535-555` separately rounds the
reported erase envelope to 4096-byte sectors.

Each Flash Data command contains a 16-byte control header followed by the
complete block. The final command therefore carried a data-length field of
1024 and all 1024 data bytes. The ROM acknowledged the sequence and esptool
continued to MD5 verification.

Local ROM linker metadata exposes ROM Flash write/program symbols, but the
binary ROM handler source for `ESP_FLASH_DATA` is not available. The host
payload and acknowledgement are confirmed; whether that ROM implementation
suppresses all-`0xFF` pages or chunks internally is unverified. The local
ESP-IDF MXIC OPI driver is corroborating, not identical ROM-handler, evidence:
it splits writes by 256-byte pages and does not scan for or skip all-`0xFF`
data before issuing Page Program.

## 11. MD5 Verification Scope

The verification path is exact:

1. `cmds.py:622-623` computes host MD5 after four-byte/address preparation,
   before final 1024-byte block padding, and records `uncsize`.
2. For this aligned candidate, that byte sequence is exactly the original
   160832-byte file.
3. `loader.py:1349-1364` asks ROM for MD5 of
   `(address=0x00020000, size=160832)`.
4. `cmds.py:743-755` prints `Hash of data verified` only when that returned
   digest equals the host digest.

Thus `Hash of data verified` means:

> ROM's MD5 over Flash `0x00020000-0x0004743F` matched MD5 of the exact
> 160832-byte prepared candidate.

It does **not** include or verify:

- padding `0x00047440-0x000477FF`;
- erase-only tail `0x00047800-0x00047FFF`;
- other partitions;
- candidate bootability;
- USB enumeration or console output;
- ready marker, stability, or Task 3.4.

The MD5 does include the candidate's own appended 32-byte SHA-256 because
that appended hash is part of the 160832-byte file. It does not recalculate
the ESP image SHA-256 semantics; it is a separate byte-for-byte range check.

## 12. NOR Flash Programming Semantics

The recorded RDID `C2 80 39` maps in Macronix Table 6 to
MX25UM25645G, a 1.8 V, 256 Mbit Octal NOR Flash with 4096-byte sectors and
256-byte pages.

The official datasheet states:

- Sector Erase erases the selected sector data to logical `1`;
- Page Program programs memory toward logical `0`;
- WREN sets volatile WEL; WIP/WEL clear after the operation;
- P_FAIL/E_FAIL are volatile status flags;
- BP3:BP0 and TB are non-volatile protection fields;
- ODS and CR2 SPI/STR-OPI mode fields used by candidate startup are volatile;
- built-in ECC is generated automatically during Page Program for 16-byte
  chunks;
- repeated programming of a chunk without erase can disable ECC until the
  containing sector is erased.

For an already successfully erased main-array byte, `0xFF` is eight `1`
bits. A request to program `0xFF` contains no zero bit and therefore requests
no user-data transition from erased `1` to programmed `0`. This is a logical
data-bit conclusion. It is not proof that a Page Program command was skipped
or that no internal physical work occurred.

The local generic and MXIC OPI drivers corroborate that a non-empty write can
still issue Program Page commands without checking whether every byte is
`0xFF`. Therefore “the data value stays `0xFF`” and “no program command or
physical side effect occurs” are different claims.

## 13. Persistent-Effect Classification

### CONFIRMED

- Installed esptool formed and passed a complete 1024-byte final block
  containing 64 candidate bytes and 960 `0xFF` bytes.
- The historical padding and erase-only tail contained 960 and 2048
  non-`0xFF` bytes respectively and belonged to the valid historical App.
- The sector erase envelope included both ranges.
- At NOR logical data semantics, a successfully erased `0xFF` byte receives
  no requested main-array `1`-to-`0` data-bit transition when the program
  data is also `0xFF`.
- The padding begins exactly after the candidate's valid appended hash and
  therefore cannot change the candidate header, segments, checksum, or
  appended hash bytes.
- WEL, WIP, P_FAIL, and E_FAIL are volatile status fields, not persistent
  main-array data.

### STRONGLY SUPPORTED

- The recorded successful erase/write sequence changed the historical
  3008-byte tail to the erased logical value, and the later `0xFF` padding
  introduced no additional user-visible main-array data-bit change in the
  first 960 bytes.
- ROM accepted and processed every complete 1024-byte Flash Data block.
- Under the standard non-Secure-Boot simple-hash path, the padding and
  erase-only tail cannot affect candidate image parsing because they lie
  after the parser-derived image end.
- A normal startup of this exact candidate performs no intentional
  main-array, partition, NVS, OTA, PHY-calibration, filesystem, or core-dump
  write.

### UNVERIFIED

- Actual post-attempt bytes in the padding and erase-only tail, because no
  readback occurred.
- Whether ESP32-S3 ROM skips any all-`0xFF` page/chunk or issues Page Program
  across the whole block.
- Internal ECC/parity or other vendor metadata state created by Page Program
  of `0xFF`.
- Exact program stress, incremental wear, program disturb, charge movement,
  retention effect, or neighboring-cell effect.
- Whether issuing a combined startup WRSR/WRCR command with unchanged
  non-volatile BP/TB values has any physical non-volatile register-cycle
  effect.
- Exact differences in the preserved `v5.5.3-dirty` vendor bootloader.

The strongest safe wording is therefore:

> No additional logical main-array data-bit change from the 960-byte
> `0xFF` padding is expected beyond the preceding erase, but absence of every
> physical or internal persistent side effect is not proven.

## 14. Candidate Image Parsing Boundary

Offline parsing found:

| Structure | Relative file boundary |
|---|---|
| image header | starts `0x00000000` |
| last segment header | `0x000273EC` |
| last segment data | ends `0x00027414` |
| checksum-aligned footer | ends `0x00027420` |
| appended SHA-256 | `0x00027420-0x0002743F` |
| valid file/image end-exclusive | `0x00027440` |
| absolute valid end-exclusive | `0x00047440` |

Therefore `0x00047440` is strictly the first byte after the legal candidate
image. The 960-byte transport padding does not shift, overwrite, or become
part of any candidate segment, checksum byte, or appended hash.

## 15. Bootloader Verification Boundary

Local ESP-IDF v5.5.4
`components/bootloader_support/src/esp_image_format.c` shows the parser:

- reads the fixed image header;
- iterates exactly `segment_count` segments and their declared lengths
  (`process_segments()`, lines 575-595);
- derives the checksum read boundary by aligning the parsed segment end plus
  checksum to 16 bytes (`process_checksum()`, lines 1015-1037);
- if `hash_appended` is set, reads exactly 32 more bytes and adds them to
  `image_len` (`process_appended_hash_and_sig()`, lines 966-975);
- compares the calculated simple hash with that appended digest
  (`verify_simple_hash()`, lines 1154-1173).

For the standard non-Secure-Boot simple-hash path, the derived boundary is
exactly 160832 bytes. That path neither needs nor verifies the later 960
bytes. The 2048-byte erase-only tail is further beyond the same candidate
boundary.

There is a separate conditional path. If a preserved bootloader is built to
enforce Secure Boot v2 signatures, `verify_secure_boot_signature()` aligns the
end after the simple hash to a 4096-byte boundary and includes that alignment
padding in the signature digest
(`esp_image_format.c:1096-1149`). It then expects a valid signature block.
That path can therefore read/hash `0x00047440-0x00047FFF`, which includes both
the 960-byte transmitted padding and the 2048-byte erase-only tail. All 3008
bytes are expected to be `0xFF` after erase, but this candidate contains no
reviewed Secure Boot signature. The exact preserved `v5.5.3-dirty`
bootloader configuration and current enforcement state remain unverified;
such enforcement would be a candidate-rejection risk.

An erased `0xFF` tail after an ESP-IDF App image is a normal unused-partition
state. It does not by itself affect App startup. This does not make the loss
of the historical Xiaozhi bytes reversible; Level 1 rollback would still be
required to restore the original complete `ota_0`.

## 16. Startup-Stage Write Audit

### ROM Bootloader

Normal reset ROM behavior selects and loads the preserved second-stage
bootloader. No source-backed ROM path was found that intentionally writes an
App partition merely to boot it. The exact ROM binary implementation is not
available, so this is strongly supported rather than direct source proof.

### Preserved vendor second-stage bootloader

The current pre-write snapshot had `otadata` entry 0 as sequence 1,
`VALID`, CRC-valid, selecting `ota_0`; entry 1 was erased. The App-only erase
and write envelope did not overlap `otadata`.

Standard ESP-IDF v5.5.4
`bootloader_support/src/bootloader_utility.c:379-452`:

- reads both OTA entries;
- writes `PENDING_VERIFY` to `ABORTED` only when rollback is enabled;
- writes `NEW` to `PENDING_VERIFY` only when rollback is enabled;
- does not write a `VALID` entry in the normal selection branch.

Standard ESP32-S3 second-stage initialization also calls
`bootloader_flash_unlock()` unconditionally
(`bootloader_flash_config_esp32s3.c:272-305`). The default MXIC path reads the
status register and skips WRSR when no BP bit needs to change; if BP bits are
set, it issues WREN/WRSR to clear them
(`bootloader_flash.c:724-790`). BP3:BP0 are non-volatile in the
MX25UM25645G. The preserved bootloader had already booted the original device,
and the later App-range erase/write succeeded without a protection failure,
so an already-unlocked logical state is strongly supported. The status
register was not read after the attempt, however, and the exact vendor-dirty
implementation is unavailable. A conditional protection-register write must
therefore be disclosed rather than asserted impossible.

With standard behavior and the recorded `VALID` state, one startup does not
modify `otadata`. Candidate and original Apps both declare secure version 0.
If anti-rollback were enabled, local source calls the secure-version update
path for a `VALID` image, but
`components/efuse/src/esp_efuse_fields.c:67-92` writes new eFuse bits only
when the App secure version is greater than the hardware value. The
preserved bootloader previously accepted the original secure-version-0 App,
and this attempt did not alter eFuse.

Residual limitation: the exact `v5.5.3-dirty` bootloader configuration and
vendor modifications are unavailable. A non-standard OTA or security write
cannot be absolutely excluded without executing or disassembling that exact
bootloader path. No such write is predicted from the known state.

### ESP-IDF candidate startup runtime

The final build component closure contains no `nvs_flash`, `esp_phy`, Wi-Fi,
Bluetooth, `esp_event`, wear levelling, filesystem, or `espcoredump`
component. Generated configuration has App/bootloader rollback, Secure Boot,
and Flash Encryption off. The ELF contains no defined mutation symbol for:

- OTA begin/write/end, boot-partition selection, or valid/invalid marking;
- partition or raw Flash write/erase;
- NVS initialization, set, or commit;
- PHY calibration initialization;
- Wi-Fi, Bluetooth, or default event-loop creation;
- wear levelling, SPIFFS, or FAT;
- core-dump writing.

Normal Octal-Flash startup does call
`spi_flash_init_chip_state()` and `esp_opiflash_init()`. For the detected MXIC
path, local source:

- reads status/configuration;
- sends WREN plus WRSR/WRCR to set ODS to 7 while preserving the read status
  byte and configuration upper bits;
- sends WREN plus WRCR2 at address 0 to select STR OPI;
- configures controller/cache state.

The MX25UM25645G datasheet classifies ODS and the CR2 SOPI/DOPI fields as
volatile. No main-array address is programmed by these register commands.
Because WRSR/WRCR also transports the unchanged non-volatile BP/TB values,
the future startup review must disclose this register cycle and the
unverified physical effect of rewriting unchanged protection fields.

### Candidate `app_main`

`firmware/main/main.c` performs only fixed `puts`/`printf` output, then an
infinite one-second `vTaskDelay` loop. Its object has exactly three undefined
runtime references: `printf`, `puts`, and `vTaskDelay`. It initializes no
peripheral and invokes no persistence API.

### Panic and reset

Generated configuration selects print-and-reboot on panic. The panic handler
can therefore reboot, potentially causing a boot loop if startup fails. Core
dump is not enabled, the component is absent, and no core-dump symbol is in
the ELF, so a panic has no candidate core-dump-to-Flash path. A reboot remains
a volatile/control-flow effect and could repeat vendor bootloader and volatile
Flash-register initialization.

## 17. Option A: Startup Observation

Proposed future scope, not authorized here:

- one independent startup reset;
- no intentional Flash array write;
- USB Serial/JTAG read-only observation for 60 seconds;
- 15-second ready-marker deadline;
- stop on failure, without automatic rollback.

Assessment:

- intentional main-array/partition bytes written: 0;
- standard `VALID` OTA selection does not write `otadata`;
- standard second-stage Flash unlock should skip WRSR when BP bits are already
  clear, but a conditional non-volatile BP clear remains a disclosed
  vendor/status uncertainty;
- candidate has no NVS, OTA, PHY, core-dump, filesystem, network, or raw
  Flash-write path;
- startup does issue documented Flash configuration-register commands for
  volatile ODS and STR OPI state, with the combined-register limitation
  described above;
- USB may disappear and re-enumerate under a different host instance/port;
- failure can present as silence, continuous ROM output, image rejection,
  signature/security rejection, panic/reboot loop, missing ready marker, or
  port loss;
- success would provide the first direct evidence that the preserved vendor
  bootloader loads the candidate, the App reaches `app_main`, USB
  Serial/JTAG works, and the ready marker appears;
- a successful bounded observation can resolve the runtime portion of Task
  3.4, but Task 3.4 must not be completed until the actual record is reviewed.

Primary risk: a reset commits to an unobserved boot path whose exact
`v5.5.3-dirty` bootloader behavior is not fully source-auditable, and USB
re-enumeration may make observation inconclusive. This risk is bounded and
more informative than another write.

## 18. Option B: Level 1 Rollback

Proposed future scope, not authorized here:

- erase and program the complete original `ota_0`;
- artifact size `0x003F0000` / 4128768 bytes;
- range `0x00020000-0x0040FFFF`;
- 1008 4 KiB sectors and 4032 1024-byte ROM blocks;
- separate startup reset and observation afterward.

Assessment:

- it is a large new erase/program operation, not a read-only recovery step;
- it introduces substantially more erase/program activity and wear than
  Option A;
- it requires a new, exact Flash authorization and an independently reviewed
  retry policy;
- stock esptool has two whole-image attempts and defaults to three block
  attempts unless a future mechanism overrides and validates them;
- staged D:/E: rollback artifacts currently match, but future authorization
  must rehash the selected artifact, backup provenance, target, range, and
  package immediately before any operation;
- it still requires reset/observation to establish original-device recovery;
- rolling back before ever starting the candidate loses the only direct
  opportunity to determine whether the App-only candidate is boot-compatible.

Primary risk: a much larger additional Flash write can fail partway or incur
retry behavior, leaving recovery still dependent on another reviewed action.
There is no current evidence that candidate bytes are corrupt or that padding
made the candidate semantically invalid, so immediate rollback is not
proportionate as the next technical step.

## 19. Option C: Continue Freeze

Keeping the device in its last reported ROM Bootloader state adds no planned
Flash array write and is the correct interim condition while no authorization
exists.

Risks and limits:

- the state is operationally fragile rather than a durable test result;
- USB disconnection, power loss, host reset signaling, or a manual reset can
  end the held ROM session;
- subsequent behavior depends on boot straps and reset conditions and may
  boot the candidate or re-enter ROM;
- leaving hardware powered in ROM indefinitely provides no new compatibility,
  console, or recovery evidence;
- the Change cannot finish in this state: a future startup observation or
  rollback plus recovery observation is still required.

Primary risk: prolonged freeze defers rather than resolves the boot/recovery
decision and increases the chance of an uncontrolled reset or loss of the
known host session state. Freeze remains mandatory until a future action is
explicitly authorized, but it is not the recommended next review outcome.

## 20. Risk Comparison

| Option | Persistent writes | Flash bytes affected | New authorization required | Information gained | Recovery complexity | Primary risk | Recommendation | Reason |
|---|---|---:|---|---|---|---|---|---|
| A. One startup reset + 60-second observation | no intentional main-array/partition write; conditional bootloader BP-register clear if protection is unexpectedly set; volatile MXIC configuration-register commands; exact unchanged-field physical effect unverified | 0 array bytes | independent startup/reset and read-only observation authorization acknowledging automatic register initialization, conditional status normalization, and vendor-bootloader residual risk | bootloader acceptance, USB console, ready marker, 60-second stability, runtime evidence for Task 3.4 | low; failure stops without automatic rollback | USB re-enumeration, silence/reboot loop, unexpected BP state, vendor-dirty residual behavior | **recommended for authorization review** | highest information per added array-write risk; no evidence candidate is corrupted |
| B. Immediate Level 1 rollback | complete `ota_0` erase/program | 4128768 bytes / 1008 sectors | independent Level 1 Flash authorization plus later startup observation authorization | restores intended original App bytes if successful; gives no candidate-runtime result | medium/high; partial failure and retries require controlled recovery | large new write, wear, retry/partial-failure risk | not recommended next | disproportionate without candidate corruption evidence; still needs observation |
| C. Continue freeze | none while state remains undisturbed | 0 | none to remain frozen; later A or B authorization still required | none | deferred | uncontrolled reset/power loss and unresolved Change | required interim state, not recommended final path | safe pause but cannot close Task 3.4 or recovery |

## 21. Recommended Next Action

**RECOMMEND STARTUP-ONLY AUTHORIZATION REVIEW**

Reasons:

1. The 960-byte padding does not alter candidate image semantics.
2. No additional logical main-array `1`-to-`0` data-bit change is expected
   from programming `0xFF` over the just-erased padding range.
3. Actual physical side effects are bounded and explicitly classified rather
   than denied.
4. Standard startup with `VALID` `otadata` and this candidate has no
   intentional partition/NVS/OTA/core-dump write path.
5. Automatic MXIC register initialization and vendor-dirty uncertainty can be
   precisely disclosed in the future startup-only review.
6. Startup observation supplies decisive compatibility information without
   another App-array erase/program.
7. Immediate rollback adds a 4128768-byte write and still requires a later
   startup observation.

The device must remain frozen until the user separately reviews and approves
or rejects that future startup-only scope.

## 22. Required Future Authorization

This section is **non-executable review guidance only**. It is not a template,
token, command, approval, or authorization material.

A future independent startup-only review would need to bind:

- exact PCB V1.0 device identity and future-confirmed observation endpoint;
- current last-known ROM Bootloader state and the stopped/inconclusive attempt
  record;
- exactly one explicit startup reset;
- exactly one bounded read-only USB Serial/JTAG observation window;
- 60-second total observation and 15-second ready-marker deadline;
- expected identity lines and exact ready marker;
- permission for normal automatic volatile/controller state changes,
  including MXIC ODS/STR-OPI register initialization;
- acknowledgment that standard second-stage Flash unlock conditionally clears
  non-volatile BP bits if an unexpected protected state is present;
- acknowledgment that combined register writes preserve logical BP/TB values
  but their unchanged-field physical effect is unverified;
- acknowledgment of the unaudited `v5.5.3-dirty` bootloader residual risk;
- stop conditions for silence, port loss, ROM loop, image/checksum/hash or
  anti-rollback rejection, panic, reset loop, watchdog, missing marker, or
  unexpected persistent-write evidence;
- no automatic retry, extended observation, readback, Flash write, erase,
  rollback, restore, monitor session, or peripheral test;
- explicit statement that failure stops and does not automatically authorize
  Level 1 rollback.

If the user does not accept the disclosed automatic register behavior or
vendor-bootloader uncertainty, the correct state is continued freeze pending
a different review. Level 1 rollback requires a separate Flash authorization.

## 23. Task 3.4 Implications

Task 3.4 remains **NOT COMPLETED**.

This report strengthens host-side evidence that:

- the candidate has a valid self-contained image boundary;
- final-block padding does not change its semantic image;
- the preserved `VALID` OTA selection should select `ota_0` without an OTA
  state mutation under standard behavior;
- the candidate has no intended persistent storage path.

It does not prove that the exact preserved `v5.5.3-dirty` bootloader loads the
candidate or that the App reaches its ready marker. Only an actually
authorized, bounded startup observation can supply that runtime evidence.
OpenSpec progress therefore remains 38/48.

## 24. Claims Explicitly Not Made

This report does not claim:

- that every affected physical Flash cell has been read and verified;
- that programming `0xFF` has absolutely no physical, ECC, wear, disturb, or
  internal-metadata effect;
- that ROM skipped the `0xFF` padding;
- that every possible preserved-bootloader security configuration ignores
  the bytes after the simple-hash boundary;
- that the candidate has booted;
- that USB Serial/JTAG will enumerate or retain the same port;
- that `otadata`, security state, or other partitions were read after the
  attempt;
- that the vendor-dirty bootloader is identical to local v5.5.4 source;
- that Task 3.4, runtime acceptance, First Flash PASS, recovery, or restoration
  is complete;
- that startup, readback, rollback, or any device action is authorized;
- that the stopped attempt should be reclassified as PASS.

## 25. Source References

Repository evidence:

- `PROJECT_CONSTITUTION.md`
- `docs/HARDWARE_PROFILE.md`
- `docs/PRODUCT_SPEC_DRAFT.md`
- `tests/hardware/pcb-v1-first-flash-attempt-2026-07-26.md`
- `docs/hardware/pcb-v1-first-flash-operation-readiness.md`
- `docs/hardware/pcb-v1-first-flash-review-package.md`
- `docs/hardware/pcb-v1-first-flash-human-authorization-review.md`
- `docs/hardware/pcb-v1-esptool-single-attempt-execution-mechanism.md`
- `docs/hardware/pcb-v1-app-only-compatibility-assessment.md`
- `docs/hardware/pcb-v1-current-boot-chain-readonly-snapshot.md`
- `tests/build/pcb-v1-first-flash-smoke-test-host-build.md`
- `firmware/main/main.c`
- `firmware/sdkconfig`
- `firmware/build/config/sdkconfig.h`
- `firmware/build/project_description.json`
- `firmware/build/pcb_v1_first_flash_smoke_test.map`
- `firmware/build/pcb_v1_first_flash_smoke_test.elf`

Installed esptool v4.12.dev3:

- `esptool/cmds.py:535-555,598-623,639-681,734-755`
- `esptool/loader.py:260,984-1040,1349-1364`
- `esptool/util.py:52-57`
- `esptool/targets/esp32.py:327-328`

Local ESP-IDF v5.5.4:

- `components/bootloader_support/src/esp_image_format.c:160-235,547-595,966-1037,1096-1173`
- `components/bootloader_support/src/bootloader_utility.c:310-350,379-458`
- `components/bootloader_support/bootloader_flash/src/bootloader_flash_config_esp32s3.c:272-305`
- `components/bootloader_support/bootloader_flash/src/bootloader_flash.c:724-793`
- `components/efuse/src/esp_efuse_fields.c:52-92`
- `components/spi_flash/flash_ops.c:179-189`
- `components/spi_flash/esp32s3/spi_flash_oct_flash_init.c:110-219,262-295`
- `components/spi_flash/spi_flash_chip_generic.c:310-359`
- `components/spi_flash/spi_flash_chip_mxic_opi.c:65-85,257-309,412`
- `components/esp_system/panic.c:405-435`
- `components/esp_system/startup.c:200-224`
- `components/freertos/app_startup.c:203-208`

Flash manufacturer evidence:

- Macronix, *MX25UM25645G, 1.8V, 256M-BIT [x1/x8] CMOS Octaflash
  Memory*, revision 1.1, June 14, 2019:
  `https://www.macronix.com/Lists/Datasheet/Attachments/8707/MX25UM25645G%2C%201.8V%2C%20256Mb%2C%20v1.1.pdf`
  (Table 6 RDID; sections 9-1 through 9-4, 10-18, 10-21, and 12-1).

## 26. Limitations

- This was intentionally host-only. There is no current readback or serial
  observation.
- Historical content proves what occupied the ranges on 2026-07-10, not an
  independently read current tail.
- ROM Flash Data handler internals and the exact vendor-dirty bootloader
  source are unavailable.
- The official Flash datasheet documents logical behavior, register types,
  ECC, and endurance but does not fully disclose internal physical
  implementation or quantify the effect of all-`0xFF` Page Program.
- Candidate build artifacts were inspected but not rebuilt; building was
  prohibited for this review.
- No binary, extracted slice, raw dump, or temporary output is added to Git.
- This report deliberately ends before any device action or authorization.
