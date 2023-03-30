#include <BluetoothSerial.h>
#include <FS.h>
#include <M5EPD.h>

M5EPD_Canvas canvas(&M5.EPD);
BluetoothSerial SerialBT;

int cnt; // set counter
int qc = 0; // queue counter
int qp = 0; // queue pointer
String queue[10];


void createTextArea() {
    canvas.createCanvas(960, 540);
    canvas.setTextArea(30, 20, 900, 500);
    canvas.createRender(90, 512);
    canvas.setTextSize(90);
}


void initCanvas() {
    M5.EPD.Clear(true);

    canvas.deleteCanvas();

    createTextArea();

    canvas.pushCanvas(0, 0, UPDATE_MODE_DU4);
    cnt = 0;
}


void printText(String text) {
    if (cnt < 1) {
        M5.EPD.Clear(true);
    }
    canvas.deleteCanvas();
    createTextArea();
    canvas.print(text);

    canvas.pushCanvas(0, 0, UPDATE_MODE_DU4);
    cnt = (cnt + 1) % 10;
    delay(100);
}


void setup() {
    M5.begin();
    M5.EPD.SetRotation(0);
    M5.EPD.Clear(true);

    createTextArea();
    canvas.drawString("Loading SD Card...", 100, 250);
    canvas.pushCanvas(0, 0, UPDATE_MODE_DU4);

    canvas.loadFont("/Arial-Unicode-Bold.ttf", SD);
    initCanvas();
    createTextArea();
    canvas.drawString("Say Something...", 100, 250);
    canvas.pushCanvas(0, 0, UPDATE_MODE_DU4);
    Serial.begin(115200);
    SerialBT.begin("Atsune's Paper");
    delay(100);
}


void loop() {
    if (SerialBT.available()) {
        String text = SerialBT.readString();
        printText(text);

        if (qc < 9) {
            queue[qc] = text;
            qc += 1;
        } else {
            for (int i = 0; i < 9; i++) {
                queue[i] = queue[i+1];
            }
            queue[qc] = text;
        }
        qp = qc;
    }
    if (Serial.available()) {
        String text = Serial.readString();
        printText(text);

        if (qc < 9) {
            queue[qc] = text;
            qc += 1;
        } else {
            for (int i = 0; i < 9; i++) {
                queue[i] = queue[i+1];
            }
            queue[qc] = text;
        }
        qp = qc;
    }
    M5.update();
    if (M5.BtnP.isPressed()) {
        cnt = 0;  
        initCanvas();
        createTextArea();
        printText(queue[qp]);   
    }
    if (M5.BtnL.isPressed()) {
        if (0 < qp) {
            qp -= 1;
            printText(queue[qp]);
        }
    }
    if (M5.BtnR.isPressed()) {
        if (qc < 9) {
            if (qp < qc-1) {
                qp += 1;
                printText(queue[qp]);
            }
        } else {
           if (qp < qc) {
                qp += 1;
                printText(queue[qp]);
            } 
        }
        
    }
}
