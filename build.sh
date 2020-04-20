#!/bin/bash
BR_NAME=buildroot-2020.02
BR_FILE=${BR_NAME}.tar.bz2
BR_DL=../${BR_FILE}
DIR_NAME=LINSW_LAB_2_Michal_Rogala
set -e
if [ ! -f ${BR_DL} ] || ! ( bzip2 -q -t ${BR_DL}); then
  (  
     cd ..
     rm -f ${BR_FILE}
     wget https://buildroot.org/downloads/${BR_FILE}
     cd ${DIR_NAME}
  )
fi
cd ..
tar -xjf ${BR_FILE}
cp ${DIR_NAME}/BR_config ${BR_NAME}/.config
cp -r ${DIR_NAME}/package/. ${BR_NAME}/package/
cd ${BR_NAME}
for i in ../LINSW_LAB_2_Michal_Rogala/patches/* ; do
   patch -p1 -N < $i
done
make
