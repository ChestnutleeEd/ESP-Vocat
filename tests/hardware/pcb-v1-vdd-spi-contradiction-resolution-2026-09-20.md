# PCB V1.0 VDD_SPI Contradiction Resolution — 2026-09-20

> **RESULT: PASS — VOLTAGE CONTRADICTION RESOLVED**
>
> **NO DEVICE WAS ACCESSED DURING THIS RESOLUTION AUDIT**
>
> **HARDWARE/DISPLAY STATUS: UNVERIFIED**

- Change: `validate-pcb-v1-display-backlight-on-hardware`
- Branch: `feat/validate-pcb-v1-display-backlight-on-hardware`
- Reviewed HEAD: `019e98f7b367b45f2b2d4a52aef0757930a8aad8`
- Scope: host-only reconstruction of existing sanitized device evidence,
  installed-tool source audit, authoritative-document review, and evidence
  reconciliation
- New COM-port opens: zero
- New resets: zero

## 1. Original observation and exact provenance

The 2026-09-20 pre-write identity gate executed this exact historical
read-only invocation once:

```powershell
& 'C:\Espressif\tools\python\v5.5.4\venv\Scripts\python.exe' -m esptool --chip esp32s3 --port COM7 --baud 115200 --before default_reset --after no_reset --no-stub --connect-attempts 1 flash_id
```

The wrapper sanitized MAC output and retained exit code 0. The relevant direct
tool output was:

```text
Manufacturer: c2
Device: 8039
Detected flash size: 32MB
Flash type set in eFuse: octal (8 data lines)
Flash voltage set by eFuse to 3.3V
Staying in bootloader.
```

Therefore `3.3V` came directly from esptool.py `v4.12.dev3` `flash_id`. It was
not printed by espefuse, derived from the application image header, or invented
by the evidence record. The original gate then conservatively interpreted the
line as a material contradiction and stopped before constructing a write
command. That fail-closed decision remains historically correct for the
information reviewed at that moment.

`flash_id` did not print the individual `VDD_SPI_FORCE`, `VDD_SPI_XPD`, or
`VDD_SPI_TIEH` values.

## 2. Existing bit-level eFuse evidence

The repository already contained a separately authorized, successful,
sanitized minimum eFuse summary from the same established PCB V1.0 unit and
`COM7`. Its authorization and execution predate this audit and are recorded in
`docs/hardware/pcb-v1-device-readonly-inspection-result.md`.

The historical wrapper connected with espefuse.py `v4.12.dev3`, selected only
ten named fields in JSON format, hard-reset normally, and exited 0. Its
effective read-only summary arguments were:

```text
--chip esp32s3 --port COM7 summary --format json
VDD_SPI_FORCE VDD_SPI_XPD VDD_SPI_TIEH FLASH_TYPE PKG_VERSION
FLASH_CAP FLASH_VENDOR PSRAM_CAP PSRAM_VENDOR PSRAM_CAP_3
```

The preserved raw logical values are:

| Field | Raw value | Decoded value | Write-protection state at query time |
|---|---:|---|---|
| `VDD_SPI_FORCE` | `0x1` | `true` | `writeable: true` |
| `VDD_SPI_XPD` | `0x1` | `true` | `writeable: true` |
| `VDD_SPI_TIEH` | `0x0` | `VDD_SPI connects to 1.8 V LDO` | `writeable: true` |

Because this bit-level evidence is sufficient to classify the device as Case
A, the newly authorized optional eFuse query was not used. No new device
interaction was necessary.

`writeable: true` is recorded only as historical readout metadata. It grants
no permission to burn, protect, or otherwise change any eFuse. It means the
summary did not report those three individual fields as write-protected at the
time of that historical query.

## 3. Installed esptool interpretation

The installed environment is:

- Python: `C:\Espressif\tools\python\v5.5.4\venv\Scripts\python.exe`;
- esptool/espefuse version: `4.12.dev3`;
- installed target definitions:
  `C:\Espressif\tools\python\v5.5.4\venv\Lib\site-packages\esptool`.

For ESP32-S3, `flash_id` calls `esp.get_flash_voltage()`. ESP32-S3 defines:

```text
VDD_SPI_XPD   = 1 << 4
VDD_SPI_TIEH  = 1 << 5
VDD_SPI_FORCE = 1 << 6
```

and inherits `_get_efuse_flash_voltage()` from the installed ESP32 target.
That implementation first evaluates:

```python
if efuse & (VDD_SPI_FORCE | VDD_SPI_XPD | VDD_SPI_TIEH):
    return "3.3V"
```

Its narrower `1.8V` and `OFF` branches test subsets of the same mask and are
therefore unreachable whenever any named bit is set. On this unit, nonzero
`FORCE` and `XPD` enter the broad first branch even though `TIEH=0` selects the
1.8 V LDO.

The source label `eFuse` in the old output means the installed helper did not
fall back to GPIO45 strapping. If all three masked eFuse bits were zero, this
helper would read the strapping register and report source `a strapping pin`:
GPIO45 high maps to 1.8 V and low/unconnected maps to approximately 3.3 V.
Neither `chip_id` nor another command infers a presently measured rail voltage.

Current upstream esptool source uses the correct decision order:

1. if `FORCE=0`, return no eFuse result and use the strap path;
2. if `FORCE=1` and `XPD=0`, report OFF;
3. if `FORCE=1`, `XPD=1`, and `TIEH=0`, report 1.8 V;
4. otherwise report 3.3 V.

The installed `v4.12.dev3` output difference is therefore a tool-version/source
logic defect relevant to this exact report, not a competing device state.

Reviewed upstream source:

`https://github.com/espressif/esptool/blob/master/esptool/targets/esp32.py`

## 4. Electrical interpretation

The exact decision-tree classification is **Case A**:

```text
VDD_SPI_FORCE = 1
VDD_SPI_XPD   = 1
VDD_SPI_TIEH  = 0
```

Meaning:

- eFuse control is forced;
- the internal SPI regulator is enabled;
- the selected source is the internal 1.8 V LDO;
- GPIO45 is ignored for VDD_SPI selection in this forced configuration.

This is `CONFIRMED` eFuse-configuration evidence. It is not a direct
instrument measurement of the physical PCB rail. No voltage setting or change
is required, proposed, or authorized.

## 5. Flash identity

The live RDID bytes were `C2 80 39`. Macronix's authoritative
MX25UM25645G datasheet defines exactly:

- manufacturer ID `C2`;
- memory type `80`;
- memory density `39`;
- part: `MX25UM25645G`;
- density: 256 Mbit / 33,554,432 bytes / 32 MiB;
- interface: SPI plus Octa I/O, with STR and DTR support;
- supply: 1.65 V to 2.0 V for read, erase, and program operations.

This exact three-byte match proves the identified Flash part family for the
queried device. It is consistent with, and materially supports, the forced
1.8 V VDD_SPI configuration.

Authoritative source:

`https://www.macronix.com/Lists/Datasheet/Attachments/8707/MX25UM25645G,%201.8V,%20256Mb,%20v1.1.pdf`

## 6. Module consistency

Espressif's authoritative ESP32-S3-WROOM-2 datasheet records:

- `ESP32-S3-WROOM-2-N32R16V`: 32 MB Octal SPI Flash and 16 MB Octal SPI
  PSRAM;
- ESP32-S3-WROOM-2 may contain ESP32-S3R16V;
- ESP32-S3R16V VDD_SPI is set to 1.8 V;
- when eFuse control is forced, `TIEH=0` selects the 1.8 V Flash voltage
  regulator and `TIEH=1` selects the approximately 3.3 V path.

The live evidence is consistent with that documented configuration:

- 32 MB Octal Flash;
- exact 1.8 V Macronix MX25UM25645G RDID;
- embedded 16 MB PSRAM with installed-tool label `AP_1v8`;
- forced 1.8 V VDD_SPI eFuse configuration;
- ESP32-S3 QFN56 revision v0.2 and USB Serial/JTAG.

The software evidence remains insufficient to prove the printed module label
or exact `N32R16V` SKU by itself. It does establish that no reviewed live field
now contradicts the documented module identity.

Authoritative sources:

- `https://documentation.espressif.com/esp32-s3-wroom-2_datasheet_en.pdf`
- `https://documentation.espressif.com/esp32_s3_datasheet_en.pdf`
- `https://docs.espressif.com/projects/esptool/en/latest/esp32s3/espefuse/set-flash-voltage-cmd.html`

## 7. Device interactions in this resolution audit

New physical interactions: **none**.

- COM7 opens: 0
- resets/reboots: 0
- Flash reads: 0
- Flash writes: 0
- Flash erases: 0
- eFuse reads: 0 new reads
- eFuse writes: 0
- stub uploads: 0
- RAM code execution: 0
- GPIO45 manipulation: 0
- application monitor/display test: 0
- persistent changes: 0

A host-only attempt to ask the installed espefuse module for its version used
an unsupported invocation form and tried to open the nonexistent default path
`/dev/ttyUSB0`; it failed before any device connection. It did not access
COM7, reset hardware, or create a physical interaction.

## 8. Gate decision

The voltage contradiction is:

`RESOLVED — DEVICE CONFIGURATION CONSISTENT WITH 1.8 V MEMORY`

The 2026-09-20 identity gate's original `FAIL` remains preserved as the
correct fail-closed outcome before bit-level reconciliation. This successor
audit clears only that voltage blocker and permits the pre-write identity gate
to be re-evaluated from its already completed non-voltage checks.

Construction and review of a future exact write packet under OpenSpec task 7.3
may now resume as a separate step. No write command was constructed here, no
Flash write is authorized, and execution still requires a fresh exact user
authorization after all packet fields are reviewed.

Physical hardware/display status remains `UNVERIFIED`.

`PASS — VDD_SPI CONTRADICTION RESOLVED; PRE-WRITE IDENTITY GATE MAY BE RE-EVALUATED`
