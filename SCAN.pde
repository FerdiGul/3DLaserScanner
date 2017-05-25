import codeanticode.gsvideo.*;
import processing.serial.*;

//Nesneler
PFont f;
GSCapture cam;
Serial myPort;
PrintWriter output;

//Renk Belirleme
color black=color(0);
color white=color(255);

//Değerler
int itr; //iterasyon
float pixBright;
float maxBright=0;
int maxBrightPos=0;
int prevMaxBrightPos;
int centerPoint=1;
int row;
int col;

//Tarama parametreleri
float odl = 210;  //webcam ve döner tabaka arasındaki mesefe
float etap = 120;  //devir başına profilendirme fazı
float camLasAngle = 25*PI/180;  //kamera ve lazer arasındaki açı
float twoProfilAngle=2*PI/etap;  //iki profil arasındaki açımız

//Koordinat Düzlemi Ayarları 
float x, y, z;  //Kartezyan Kordinatları., [milimetre]
float ro;  //ilk polar kordinatımız
float fi; //ikinci polar kordinatımız [radian]
float b; //fotoğrafın ortası ve brightes arasındaki mesafe[pixel]
float pxmmpoz = 5; //mm cinsinden yatay mesafe 1px=0.2mm
float pxmmpion = 5; //mm cinsinden dikey mesafe 1px=0.2mm

//================= CONFIG ===================

void setup() {
  size(800, 600);
  strokeWeight(1);
  smooth();
  background(0);
 
  //FONT AYARI
  f=createFont("Arial",16,true);
   
  //KAMERA AYARLARI
  String[] camera=GSCapture.list();
  if (camera.length==0){
    println("Resim çekimi için geçerli kamera algılanamadı ");
    textFont(f,12);
    fill(255,0,0);
    text("Kamera Hazır Değil !",680,32);
  }
  else{
    println("Görünen Kameralar:");
    for (int i = 0; i < camera.length; i++) {
      println(camera[i]);
    }
    textFont(f,12);
    fill(0,255,0);
    text("KAMERA HAZIR",680,32);
    cam=new GSCapture(this, 640, 480,camera[0]);
    cam.start();
  }
 
  //Serial (COM) AYARI
  println(Serial.list());
  myPort=new Serial(this, Serial.list()[0], 9600);
 
  //KAYDEDİLECEK TARAMA DOSYASININ ADI VE UZANTISI (.txt denilebilir.)
  output=createWriter("skan.asc");  
 

 
}

//============== ANA PROGRAM =================

void draw() {
 
  PImage photo=createImage(cam.width,cam.height,RGB);
  cam.read();
  delay(2000);
  for (itr=0;itr<etap;itr++) {
    cam.read();
    photo.loadPixels();
    cam.loadPixels();
    for (int n=0;n<photo.width*photo.height;n++){
      photo.pixels[n]=cam.pixels[n];
    }
    photo.updatePixels();
    set(20,20,cam);
    String getFile="photo-"+nf(itr+1, 3)+".png";
    photo.save(getFile);
    turning();
    delay(500);
  }
  turning();
  licz();
  noLoop();
 
}

void licz(){
  for (itr=0; itr<etap; itr++){
   
    String getFile="photo-"+nf(itr+1, 3)+".png";
    PImage skan=loadImage(getFile);
    String getFile2="photoProcess-"+nf(itr+1, 3)+".png";
    PImage photoProcess=createImage(skan.width, skan.height, RGB);
    skan.loadPixels();
    photoProcess.loadPixels();
    int currentPos;
    fi=itr*twoProfilAngle;
    println(fi);

    for(row=0; row<skan.height; row++){  //SATIR ANALİZİ BAŞLANGICI
    maxBrightPos=0;
    maxBright=0;
      for(col=0; col<skan.width; col++){
        currentPos = row * skan.width + col;
        pixBright=brightness(skan.pixels[currentPos]);
        if(pixBright>maxBright){
          maxBright=pixBright;
          maxBrightPos=currentPos;
        }
        photoProcess.pixels[currentPos]=black; //BÜTÜN PİXELLERİ SİYAH YAPMA
      }
     
      photoProcess.pixels[maxBrightPos]=white; //BÜTÜN PİXELLERİ BEYAZ  YAPMA
     
      b=((maxBrightPos+1-row*skan.width)-skan.width/2)/pxmmpoz;
      ro=b/sin(camLasAngle);
      //output.println(b + ", " + prevMaxBrightPos + ", " + maxBrightPos); //DEBUG için kullanınız
     
      x=ro * cos(fi);  //polar kordinatları kartezyana dönüştürme
      y=ro * sin(fi);
      z=row/pxmmpion;
     
      if( (ro>=-30) && (ro<=60) ){ //yazma kordinatları
        output.println(x + "," + y + "," + z);
      }
     
    }//SATIR ANALİZİ SONU
   
    photoProcess.updatePixels();
    photoProcess.save(getFile2);
   
  }
  output.flush();
  output.close();
}

void turning() {  //dönüş fonksiyonu
  myPort.write('S');
  delay(50);
  myPort.write('K');
}
