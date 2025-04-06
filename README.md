// 获得文件大小
./ftp_breakpoint -m i -u ftpuser:84436446 -s ftp://ftpuser@47.119.134.66/src_10m.bin

// 下载文件
./ftp_breakpoint -m d -u ftpuser:84436446 -s ftp://ftpuser@47.119.134.66/src_10m.bin -f ./src_10m.bin -b

// 上传文件
./ftp_breakpoint -m u -u ftpuser:84436446 -s ftp://ftpuser@47.119.134.66/test.bin -f ./src_10m.bin -b

// 使用命令下载
curl -v -u ftpuser:84436446 -O ftp://ftpuser@47.119.134.66/123.txt

// 使用命令上传
curl -T test.bin -u ftpuser:84436446 ftp://ftpuser@47.119.134.66/test1.bin
