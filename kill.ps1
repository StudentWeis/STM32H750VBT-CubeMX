# Keil STM32 编译输出

Get-ChildItem * -Include *.axf -Recurse | Remove-Item
Get-ChildItem * -Include *.d -Recurse | Remove-Item
Get-ChildItem * -Include *.dep -Recurse | Remove-Item
Get-ChildItem * -Include *.lnp -Recurse | Remove-Item
Get-ChildItem * -Include *.map -Recurse | Remove-Item
Get-ChildItem * -Include *.htm -Recurse | Remove-Item
Get-ChildItem * -Include *.o -Recurse | Remove-Item
Get-ChildItem * -Include *.sct -Recurse | Remove-Item
Get-ChildItem * -Include *.lst -Recurse | Remove-Item
Get-ChildItem * -Include *.Bak -Recurse | Remove-Item
Get-ChildItem * -Include *.hex -Recurse | Remove-Item

# Keil 工程输出

Get-ChildItem * -Include *.uvguix.lenovo -Recurse | Remove-Item