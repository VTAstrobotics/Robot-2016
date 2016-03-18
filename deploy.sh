#! /bin/sh

SSHCMD='ssh -T root@10.0.0.30'

echo "Backing up existing sketch on target..."
echo 'mv -f /sketch/sketch.elf /sketch/sketch.elf.old' | $SSHCMD
echo "Stopping sketch process..."
echo 'killall sketch.elf' | $SSHCMD
echo "Sending new sketch..."
scp Release/Robot-2016.elf root@10.0.0.30:/sketch/sketch.elf
echo "Syncing to memory card..."
echo 'sync' | $SSHCMD
echo "Done"
echo "The board must now be manually rebooted (not reset)."
