import string_kernel
today = [bytes("今日は非常にいい天気だ","utf-8"), bytes("昨日はいい天気だ", "utf-8")]
yesterday=[bytes("昨日はいい天気だ", "utf-8")]
print(string_kernel.stringKernelUTF8Multi(today,yesterday, 2, 0.5))
print(string_kernel.stringKernelUTF8(today[0],yesterday[0], 2, 0.5))
