#!/bin/bash
cd $1

export MTOOLSRC=tmp/kernel4.cfg

mkdir tmp 2> /dev/null

cat << EOF > $MTOOLSRC
drive x:
   file="fd.img" cylinders=80 heads=2 sectors=18 filter
EOF

echo "CP       base.img.bz2 -> fd.img.bz2"
cp scripts/base.img.bz2 fd.img.bz2
rm fd.img 2> /dev/null
echo "BUNZIP   fd.img.bz2"
bunzip2 fd.img.bz2

install -d content/boot/grub
echo "CP       scripts/fd_menu_$2.lst -> fd.img/boot/grub/menu.lst"
cp scripts/fd_menu_$2.lst content/boot/grub/menu.lst
echo "CP       content/* -> fd.img"
mcopy -s content/* x:/

rm $MTOOLSRC

exit
