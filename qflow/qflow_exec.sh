#!/bin/tcsh -f
#-------------------------------------------
# qflow exec script for project /ef-design/projects/dwarfRV32/qflow
#-------------------------------------------

# /ef/apps/ocd/qflow/1.1.70/share/qflow/scripts/synthesize.sh /ef-design/projects/dwarfRV32/qflow rv32_CPU_v2 /ef-design/projects/dwarfRV32/qflow/source/rv32.v || exit 1
# /ef/apps/ocd/qflow/1.1.70/share/qflow/scripts/placement.sh -d /ef-design/projects/dwarfRV32/qflow rv32_CPU_v2 || exit 1
# /ef/apps/ocd/qflow/1.1.70/share/qflow/scripts/vesta.sh /ef-design/projects/dwarfRV32/qflow rv32_CPU_v2 || exit 1
# /ef/apps/ocd/qflow/1.1.70/share/qflow/scripts/router.sh /ef-design/projects/dwarfRV32/qflow rv32_CPU_v2 || exit 1
# /ef/apps/ocd/qflow/1.1.70/share/qflow/scripts/vesta.sh -d /ef-design/projects/dwarfRV32/qflow rv32_CPU_v2 || exit 1
# /ef/apps/ocd/qflow/1.1.70/share/qflow/scripts/cleanup.sh /ef-design/projects/dwarfRV32/qflow rv32_CPU_v2 || exit 1
/ef/apps/ocd/qflow/1.1.70/share/qflow/scripts/display.sh /ef-design/projects/dwarfRV32/qflow rv32_CPU_v2 || exit 1
