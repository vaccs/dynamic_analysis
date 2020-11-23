# setup paths for vaccs
# add this to your ~/.bashrc

export VACCS=`pwd`
export VPIN=$VACCS/pin
export VPAS=$VPIN/source/tools/PAS
export VPINCPP=$VACCS/vaccs_pin_cpp
export VDWARF=$VACCS/vaccs_dwarf
export PATH=$PATH:$VPIN/scripts:$VDWARF/execs/dwread:$VDWARF/execs/dwwrite
