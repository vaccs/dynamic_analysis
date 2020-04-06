# setup paths for vaccs
# add this to your ~/.bashrc

export VACCS=$HOME/vaccs
export VPIN=$VACCS/dynamic_analysis/pin
export VPAS=$VPIN/source/tools/PAS
export VPINCPP=$VACCS/dynamic_analysis/vaccs_pin_cpp
export VDWARF=$VACCS/vaccs_dwarf
export PATH=$PATH:$VPIN/scripts:$VDWARF/execs/dwread:$VDWARF/execs/dwwrite
