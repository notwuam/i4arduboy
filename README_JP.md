# i4arduboy

2Dシューティング「伊 (英題:i-20000)」の [Arduboy](https://www.arduboy.com/) 移植版です。
オリジナルのWindowsバージョンについては、[特設ページ](http://amamoriya.gozaru.jp/i)をご覧ください。

遊ぶには [Arduboy](https://www.arduboy.com/) が必要です。
しかし、他の [Arduino](https://www.arduino.cc/) ハードウェアでも工夫すれば動くかもしれません。(ソースコードの改造が必要です。[詳細](#other_hardwares))

[English document is here](./README.md).

## 動画

[プレイ動画(Youtube)](https://www.youtube.com/watch?v=KDbjBe9sMog)

## インストール方法

1. [ArduinoIDE](https://www.arduino.cc/en/Main/Software) をインストール
2. [ArduboyLib](https://github.com/Arduboy/Arduboy) をダウンロードし、先ほどのArduino環境に導入 ([詳細](https://github.com/Arduboy/Arduboy#how-to-use-the-library))
3. このリポジトリを clone かダウンロード (ZIPダウンロードの場合は展開してください)
4. Arduboy をUSBケーブルで接続
5. "i4arduboy.ino" を ArduinoIDE で開く
6. IDE左上の「マイコンボードに書き込む(右矢印マーク)」ボタンをクリック
7. 遊ぶ

## システム

* **攻撃手段**
  + **機銃** - 画面内に敵がいる場合自動で発射される
  + **魚雷** - AボタンかBボタンで発射
  + 小さい敵は機銃でも倒せるが、大きな敵は魚雷でしか倒せない
  + 魚雷は一度に一発しか撃てない
* **ゲームフィールド**
  + 見えている画面の先に、2画面分の**見えない領域**が存在
  + この領域の敵を攻撃できるのは、魚雷でのみ可能
  + 魚雷を外すと敵に位置を感知され、攻撃しようと迫ってくる
* **ソナー**
  + 画面外の敵位置は、画面右のドットによりおおまかにわかる

## 操作方法

* **方向ボタン**: 自機潜水艦の移動 / カーソル移動
* **Aボタン**: 魚雷発射 / 決定
* **Bボタン**: 魚雷発射 / キャンセル
* A+B+左 同時押し (ゲーム中のみ): タイトルに戻る(捨てゲー)

## ビルドオプション

ソースのうち、```constants.h```のマクロ定義で一部の項目についてビルド設定が可能です。

* ```LOW_FLASH_MEMORY``` - プログラム容量(FLASHメモリ)が少なくてビルドができない場合コメントを外してください。一部の機能が省略される代わり、数百Bytesの余裕ができます。
* ```USE_RANKING_EEPROM``` - 定義すると Arduino の不揮発性メモリ(EEPROM)を用い、ランキングデータが電源を切っても維持されるようになります。プログラムに数百Bytesの追加容量が必要です。
* ```DISABLE_MUSIC``` - 定義するとBGM再生を省き、500Bytes弱プログラム容量を削減できます。

ほか、各キャラクタの最大数や、残機数等の自機設定も可能です。

## <a name="other_hardwares">他のArduino環境で動作させるためには

Arduino から映像を出力する手段として、グラフィック液晶を使う、簡単な回路を作ってコンポジット出力する等があります。

i4arduboy は、```gamecore.h```の```GameCore```構造体のメンバ関数やマクロを適切に実装し直すことにより、これらの環境へゲームを表示することができるよう作られています。

* ```SCREEN_WIDTH``` - 映像表示デバイスの横画素数を設定。
* ```SCREEN_HEIGHT``` - 映像表示デバイスの縦画素数を設定。
* ```void setup()``` - 起動直後一度だけ呼ばれる。
* ```void updateInput()``` - 入力の更新。メインループの冒頭で毎回呼ばれる。
* ```bool pressed(byte button)``` - ```BTN_X```マクロで定義されたビットフラグに対応するボタンが押下中であれば```true```.
* ```bool pushed(byte button)``` - ```BTN_X```マクロで定義されたビットフラグに対応するボタンがそのフレーム押し込まれていれば```true```.
* ```bool nextFrame()``` - メインループで次の同期タイミングまで```false```を返す。
* ```void clearDisplay()``` - 画面の消去。
* ```void display()``` - 画面に描画した内容を表示。(ダブルバッファリング時のフリップ等)
* ```int frameCount()``` - ```nextFrame```が```true```になった回数。
* ```void drawPixel(int x, int y, byte c)``` - x,y の位置に1画素を描画。(cは color. 0:黒、1:白)
* ```void drawBitmap(int x, int y, const byte* bitmap, byte c)``` - PROGMEMに置かれた```byte```配列画像データを x,y を左上として描画。(cは color. 0:黒、1:白、2:白黒反転) 画像データフォーマットについては以下。
  + 0 Byte 目 - width (画像幅) 0-255
  + 1 Byte 目 - height (画像高さ) 0-255
  + 2 Byte 以降 から (width * height + 2) まで - 
    画像左上から8画素ぶん下 (yのプラス方向) への白黒データを順に格納。
    widthBytesごとに次の行ブロックとなる。
* ```void fillRect(int x, int y, int w, int h, byte c)``` - (x,y)-(x+w-1,y+h-1) の矩形を塗りつぶす。(cは color. 0:黒、1:白)
* ```void drawCircle(int x, int y, int r, byte c)``` - x,y を中心として半径 r の円を描画する。(cは color. 0:黒、1:白)
* ```void setCursor(int x, int y)``` - 文字列描画の左上位置を設定する。
* ```void print(char* text)``` - ```setCursor```で指定した位置に文字列を描画する。
* ```bool playing()``` - 楽譜データ演奏中ならば```true```を返す。
* ```void playScore(const byte* score)``` - PROGMEMに配置された```byte```配列の楽譜データを再生する。フォーマットは [miditones](https://github.com/LenShustek/miditones) によって出力されるものと同一である。
* ```void stopScore()``` - 楽譜データ演奏中ならばそれを停止する。
* ```void tone(unsigned int f, unsigned long d)``` - 周波数 f のビープを d[ms] 発声。

映像出力ライブラリの候補としては [ArduinoPlayground/Output](http://playground.arduino.cc/Main/InterfacingWithHardware#Output) の LCDs や Video Output を参考にすると良いでしょう。

## 技術情報

Coming soon.
