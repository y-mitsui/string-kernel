### ２つのテキストの類似度を文字列カーネルを使って計算するデモ ###
import string_kernel

### デモ１
print("======= DEMO1 =======")
text1 = "今日は非常にいい天気だ"
text2 = "昨日はいい天気だ"
print("テキスト１：%s"%(text1))
print("テキスト２：%s"%(text2))
# bytesに変換してから渡します
similar = string_kernel.stringKernelUTF8(bytes(text1, "utf-8"), bytes(text2, "utf-8"), 2, 0.5)
print("類似性スコア：%.4f"%(similar))

### デモ2
print("======= DEMO2 =======")
text1 = "今日は非常にいい天気だ"
text2 = "今日は非常にいい天気だ"
print("テキスト１：%s"%(text1))
print("テキスト２：%s"%(text2))
similar = string_kernel.stringKernelUTF8(bytes(text1, "utf-8"), bytes(text2, "utf-8"), 2, 0.5)
print("類似性スコア：%.4f"%(similar))

### デモ3
print("======= DEMO3 =======")
text1 = "今日は非常にいい天気だ"
text2 = "私は大きな喜びを感じている"
print("テキスト１：%s"%(text1))
print("テキスト２：%s"%(text2))
similar = string_kernel.stringKernelUTF8(bytes(text1, "utf-8"), bytes(text2, "utf-8"), 2, 0.5)
print("類似性スコア：%.4f"%(similar))

