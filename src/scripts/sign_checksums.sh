#!/bin/sh
for file in *; do echo "SHA256 Checksum: "; sha256sum $file; echo ""; done | gpg --sign --clear-sign
