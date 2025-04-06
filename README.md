
// 下载文件
./ftp_breakpoint -d -s ftp://ftpuser@47.119.134.66/src.bin -f ./test.bin -b

// 上传文件
./ftp_breakpoint -d -s ftp://ftpuser@47.119.134.66/test.bin -f ./src.bin -b

// 获得文件大小
./ftp_breakpoint -i -s ftp://ftpuser@47.119.134.66/src.bin

// 使用命令下载
curl -v -u ftpuser:84436446 -O ftp://ftpuser@47.119.134.66/123.txt

// 使用命令上传
curl -T test.bin -u ftpuser:84436446 ftp://ftpuser@47.119.134.66/
