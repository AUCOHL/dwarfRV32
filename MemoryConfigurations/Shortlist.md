# Memory configurations for DwarfRV32-based SoCs

| Scenario | Boot ROM | Internal SRAM | Internal Flash | External Flash |
| - | - | - | - | - |
| 0. Default | - | Data | NOR, Runtime & Program | -
| 1. NOR is expensive | - | Data, eventually Runtime & Program | NAND, Runtime & Program, Bootstrap to load everything into SRAM | - |
| 2. External Flash | Jump to external flash | Data, eventually Runtime & Program | - | Runtime & Program, Bootstrap to load everything into SRAM
| ?. At User's Discretion⌘  | ? | ? | ? | ? |

⌘: User has to provide link.ld and crt0.s
