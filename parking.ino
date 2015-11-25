//Initial Commit
//Changelog 2
//Changelog 3
#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>
#include <SPI.h>
#define numofspots 6
#define inputmode 1 // 0 is for Serial input, 1 for web input
const int pins []= {1,2,3,4,5,6};
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 130); // Arduino IP Add
EthernetServer server(80); // Web server
String CONTENT_LENGTH_TXT = "Content-Length: ";
// Http data
String reqData; // Request from Smartphone
String header;
int contentSize = -1;

EthernetClient client = server.available();


class ParkingSpot{
  private:
    int spotnumber;
    bool occupied = false;
    bool booked = false;
    String bookedby="";
    int pin;
  
  public:
    // Contructor; takes in a spot number and an input number
    ParkingSpot(int spot, int pin){
      spotnumber = spot;
      this->pin = pin;
    }

  // checks the pins for if the spot is physically occupied  
  void checkOccupied(){
    
  }
  //Gives in the string for the name of the booker
  void setBooked(){
    booked = true;
  }
  //Removes the booking from the parking spot
  void setUnbooked(){
    booked = false;
    bookedby = "";
  }

  bool alarm(){
    if (!booked && occupied){
      return true;
    }
    return false;
  }
  String booker(){
    return bookedby;
  }
  
};
ParkingSpot *parkinglot[numofspots];

// checks and updates the status for all parking spots
void updateAllStatus(){
  for (int i = 0; i < numofspots; i++){
    parkinglot[i]->checkOccupied();
  }
}

// initializes the parking lot
void InitializeParkingLot(){
 for (int i = 0; i < numofspots; i++){
    parkinglot[i] = new ParkingSpot (i, pins[i]);
  }
}

String GetSerialString(){
  String status;
  return Serial.readString();
}

String GetWebString(){
  String status;
  if (client){
    boolean isLastLine = true;
    boolean isBody = false;
    header = "";
    reqData = "";
    int contentLen = 0;

    Serial.print("Client Connected!");

    
          if (client.available()) {
            // Read data
            char c = client.read();
            if (contentSize == contentLen) {
              int idx = reqData.indexOf(":");
              status = reqData.substring(idx + 1, idx + 2);
              Serial.println("Status : " + status); // Status is the string coming in             
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              client.println("Connection: close");
              client.println();
              // send web page
              client.println("");
              client.println("");
              return status;
            }
             
             
            if (c == '\n' && isLastLine) {
                isBody = true;
                int pos = header.indexOf(CONTENT_LENGTH_TXT);
                String tmp = header.substring(pos, header.length());
                int pos1 = tmp.indexOf("\r\n");
                String size = tmp.substring(CONTENT_LENGTH_TXT.length(), pos1);
                Serial.println("Size ["+size+"]");
                contentSize = size.toInt();                 
            }
             
            if (isBody) {
              reqData += c;
              contentLen++;
            }
            else {
             header += c;
            }             
            if (c == '\n' ) {
             isLastLine = true;
            }
            else if (c != '\r' ) {
              isLastLine = false;
            }
             
            return status;            
    }
     
    // Close connection
    Serial.println("Stop..");
    client.stop();
  }
}

void setup() {
  InitializeParkingLot();
  updateAllStatus();
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  server.begin();
}

boolean checkAlarm(){
  for (int i = 0; i < numofspots; i++){
    if (parkinglot[i]->alarm())
      return true;
  }
  return false;
}


void loop() {
  String inputtext = GetWebString();
  char function = inputtext.charAt(0);
  char spotnum = inputtext.charAt(1);
  int spot = (int) spotnum;

  if (function == 'b'){
    parkinglot[spot]->setBooked();
  }
  else{
    parkinglot[spot]->setUnbooked();
  }

}
