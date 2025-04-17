# linux中的iptables命令：开放端口号
使用方法：
```shell
//添加端口号
sudo iptables -A INPUT -p tcp/udp --dport '端口号' -j ACCEPT
//删除端口号
sudo iptables -D INPUT -p udp/tcp --dport '端口号' -j ACCEPT

```
192.168.153.129
