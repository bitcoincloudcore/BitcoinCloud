#!/usr/bin/env bash

export LC_ALL=C
TOPDIR=${TOPDIR:-$(git rev-parse --show-toplevel)}
BUILDDIR=${BUILDDIR:-$TOPDIR}

BINDIR=${BINDIR:-$BUILDDIR/src}
MANDIR=${MANDIR:-$TOPDIR/doc/man}

BITCOINCLOUDD=${BITCOINCLOUDD:-$BINDIR/bitcoincloudd}
BITCOINCLOUDCLI=${BITCOINCLOUDCLI:-$BINDIR/bitcoincloud-cli}
BITCOINCLOUDTX=${BITCOINCLOUDTX:-$BINDIR/bitcoincloud-tx}
BITCOINCLOUDQT=${BITCOINCLOUDQT:-$BINDIR/qt/bitcoincloud-qt}

[ ! -x $BITCOINCLOUDD ] && echo "$BITCOINCLOUDD not found or not executable." && exit 1

# The autodetected version git tag can screw up manpage output a little bit
BCLVER=($($BITCOINCLOUDCLI --version | head -n1 | awk -F'[ -]' '{ print $6, $7 }'))

# Create a footer file with copyright content.
# This gets autodetected fine for bitcoincloudd if --version-string is not set,
# but has different outcomes for bitcoincloud-qt and bitcoincloud-cli.
echo "[COPYRIGHT]" > footer.h2m
$BITCOINCLOUDD --version | sed -n '1!p' >> footer.h2m

for cmd in $BITCOINCLOUDD $BITCOINCLOUDCLI $BITCOINCLOUDTX $BITCOINCLOUDQT; do
  cmdname="${cmd##*/}"
  help2man -N --version-string=${BCLVER[0]} --include=footer.h2m -o ${MANDIR}/${cmdname}.1 ${cmd}
  sed -i "s/\\\-${BCLVER[1]}//g" ${MANDIR}/${cmdname}.1
done

rm -f footer.h2m
