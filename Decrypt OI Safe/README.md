Decryp save file from App

App is OI Safe 1.4.6;  org.openintents.safe.apk Passwort: "TestTest" (ohne ")
https://github.com/openintents/safe/tree/1.4.6.1/Safe
```shell
sudo waydroid shell cat /sdcard/oisafe.xml 
sudo sqlite3 "/home/boss/.local/share/waydroid/data/data/org.openintents.safe/databases/safe" .dump
sudo cat /home/boss/.local/share/waydroid/data/media/0/oisafe.xml
sudo cat /home/boss/.local/share/waydroid/data/media/0/oisafe.csv
```
