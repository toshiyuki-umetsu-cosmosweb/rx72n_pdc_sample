# 概要

RX72N で PDC を使う実験をしたテストコードです。

# 使用ボード

（株）北斗電子 HSBRX72N176
https://www.hokutodenshi.co.jp/7/HSBRX72N176.htm

# 使用した開発環境

Renesas e2studio 2022-07
GCC for Renesas RX 8.4.0.202204
Renesas E1 Emulator

# 何を見るの？

* PDCでデータをRAM上に蓄えて、USBで取り出せるか、実験をします。
オンボードRAM上に640x480@30fpsの1フレームデータをキャプチャ操作したとき、転送が間に合うのかどうかと、
DMACの動作を確認します。

# 動作仕様

* USB Mini-B CDC でホストから操作。
* PDC 動作
  RAM1の後半256KBと、RAM2の512KBを使用してキャプチャします。
  DMAはDMAC3+CGドライバで行っています。
* GLCDCを使用してテスト信号を出力。バックグラウンドカラー(B)と同期信号のみ。
  640x480@30fps PixelClock=30MHz, HSync=15kHz, Vsync=30Hz
* I2C 動作
  カメラのイメージセンサとI2Cで接続して操作する想定で、インタフェースを設けています。 
  
# コマンド

* **args**
コマンド引数を表示します。トークン分割のテスト用です。
* **help**
コマンド一覧を表示します。
* **reset**
ソフトウェアリセットを実行します。
* **i2c bit-rate [bit-rate#]**
I2Cバスのビットレートを設定/取得します。
* **i2c slave_addr# [ send tx0# [ tx1# [ ... ] ] ] [ recv rx_len# ]**
i2c I2Cバスを介してデータを送受信します。slave_addr#は7bit形式です。
最大で16バイトまで送受信できます。
* **test-data output [on|off]**
GLCDCを使用した、テスト信号出力をON/OFFします。
* **test-data data [d#]**
テストデータ用データ値を指定します。
* **pdc capture-range**
キャプチャ範囲を設定/取得します。
* **pdc signal-polarity**
HSYNC,VSYNCの極性を設定/取得します。
* **pdc reset**
PDCをリセットします。(ステータスレジスタがリセットされます。)
PDC のリセットはPixelClock入力に同期して行われる仕様のため、
PixelClock入力がないと失敗します。
* **pdc capture**
PDCのキャプチャを実行します。
* **pdc stop**
PDCのキャプチャを停止(PCCR1.PCE=0)します。
* **pdc state**
PDCのステータスを表示します。

# I/Oメモ

## PDC

パラレルデータキャプチャユニットです。8bitカメラ入力です。
PCLKBで動作し、対応可能なPixeclClockは PixcelClock <= PCLKB * 0.6までになります。

|RX72N PIN#|PDC|Dir|HSBRX72N176 Connector|Signal|
|--:|---|:-:|:-:|---|
|40|P24/PIXCLK|In|J2.46|Pixel clock|
|38|P25/HSYNC|In|J2.45|H Sync|
|29|P32/VSYNC|In|J2.36|V Sync|
|50|P15/PIXD0|In|J2.55|Data[0]|
|49|P86/PIXD1|In|J2.54|Data[1]|
|47|P87/PIXD2|In|J2.52|Data[2]|
|46|P17/PIXD3|In|J2.51|Data[3]|
|45|P20/PIXD4|In|J2.50|Data[4]|
|44|P21/PIXD5|In|J2.49|Data[5]|
|43|P22/PIXD6|In|J2.48|Data[6]|
|42|P23/PIXD7|In|J2.47|Data[7]|
|28|P33/PCKO|Out|J2.35|Camera clock out.|

## I2C

カメラのイメージセンサ操作用に割り当て。

|RX72N PIN#|PDC|Dir|HSBRX72N176 Connector|Signal|
|--:|---|:-:|:-:|---|
|7|P00/SSDA6|I/O|J2.25|I2C Data|
|8|P01/SSCL6|Out|J2.24|I2C Clock|

## GLCDC

テスト信号出力。 640x480@30fps 30MHz。
VFP=8, VS=2, VBP=10, VACT=480,
HFP=48, HSYNC=60, HBP=612, HACT=1280
GLCDCの背景画像 B データが出続ける。
HSyncの論理は正論理(HighActive), VSyncの論理は負論理(LowActive)

|RX72N PIN#|PDC|Dir|HSBRX72N176 Connector|Signal|
|--:|---|:-:|:-:|---|
|51|P14/LCD_CLK-A|Out|J2.56|Pixel Clock|
|52|P13/TCON0-A|Out|J2.57|H Sync|
|53|P12/TCON1-A|Out|J2.58|V Sync|
|60|PJ0/LCD_DATA0-A|Out|J3.6|B[0]|
|61|P85/LCD_DATA1-A|Out|J3.7|B[1]|
|62|P84/LCD_DATA2-A|Out|J3.8|B[2]|
|63|P57/LCD_DATA3-A|Out|J3.9|B[3]|
|64|P56/LCD_DATA4-A|Out|J3.10|B[4]|
|65|P55/LCD_DATA5-A|Out|J3.11|B[5]|
|66|P54/LCD_DATA6-A|Out|J3.12|B[6]|
|67|P11/LCD_DATA7-A|Out|J2.59|B[7]|

## USB

USBデバイスとして動作する。MCUボードのMini-BコネクタとPCを接続する想定。

|RX72N PIN#|PDC|Dir|HSBRX72N176 Connector|Signal|
|--:|---|:-:|:-:|---|
|55|USB_DM|I/O|J5.D-|USB Data-|
|56|USB_DP|I/O|J5.D+|USB Data+|
|48|P16/USB0_VBUS|In|J5.VBUS|VBUS Input|


 