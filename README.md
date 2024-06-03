# Kronus Tests

```shell
dtc -@ -O dtb -o kronus-tests.dtbo dts/kronus-tests.dtso
```

```shell
mkdir -p /sys/kernel/config/device-tree/overlays/kronus-tests
cat apb-test.dtbo > /sys/kernel/config/device-tree/overlays/kronus-tests/dtbo
```

```shell
cp uio-name.rules /etc/udev/rules.d/
udevadm control --reload
udecadm trigger
```
