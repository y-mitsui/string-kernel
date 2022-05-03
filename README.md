# string-kernel
このPythonライブラリでは文字列カーネルを使った２つのテキストの類似性を表すスコアを計算することが出来ます。  
カーネル法で必要になるグラム行列をこのライブラリから返されるスコアを使って作成することが出来るようになります。  
文書分類などさまざまなテキストマイニングのタスクに有効です。  
また、このライブラリはC言語で実装後、Pythonラッピングしています。
# インストール
このライブラリはcythonを使っていますので、まだの方はインストールをお願いします。  
Ubuntuの場合のインストール手順
```
sudo python3-pip install cython
sudo python3 setup.py install
```
# 使い方
```
double sskKernelUtf8(char *s, char *t, int k, double l)
@param　s １つ目の文書
@param　t 2つ目の文書
@param　k 分割される部分文字列の最大の長さ
@param　l ギャップに対するペナルティ
@return　２つの文書の類似性スコア
```
詳しい使い方は、example.pyをご覧ください。

