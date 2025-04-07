// 获得文件大小
./ftp_breakpoint -m i -u ftpuser:84436446 -s ftp://ftpuser@47.119.134.66/src_10m.bin
./ftp_breakpoint -m i -u domchar:domchar -s ftp://domchar@116.204.66.82/src_10m.bin

// 下载文件
./ftp_breakpoint -m d -u ftpuser:84436446 -s ftp://ftpuser@47.119.134.66/src_10m.bin -f ./src_10m.bin -b
./ftp_breakpoint -m d -u domchar:domchar -s ftp://domchar@116.204.66.82/src_10m.bin -f ./src_10m.bin -b

// 上传文件
./ftp_breakpoint -m u -u ftpuser:84436446 -s ftp://ftpuser@47.119.134.66/test.bin -f ./src_10m.bin -b
./ftp_breakpoint -m u -u domchar:domchar -s ftp://domchar@116.204.66.82/test.bin -f ./src_10m.bin -b

// 使用命令下载
curl -v -u ftpuser:84436446 -O ftp://ftpuser@47.119.134.66/src_10.bin
curl -v -u domchar:domchar -O ftp://domchar@116.204.66.82/src_10m.bin

// 使用命令上传
curl -T test.bin -u ftpuser:84436446 ftp://ftpuser@47.119.134.66/test1.bin
curl -T src_10m.bin -u domchar:domchar ftp://domchar@116.204.66.82/test1.bin
