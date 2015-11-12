# i4arduboy

2Dシューティング「伊 (英題:i-20000)」の [Arduboy](https://www.arduboy.com/) 移植版です。
オリジナルのWindowsバージョンについては、[特設ページ](http://amamoriya.gozaru.jp/i)をご覧ください。

遊ぶには [Arduboy](https://www.arduboy.com/) が必要です。
しかし、他の [Arduino](https://www.arduino.cc/) ハードウェアでも工夫すれば動くかもしれません。(ソースコードの改造が必要です。)

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

## 他のArduino環境で動作させるためには

Coming soon.

## 技術情報

Coming soon.
