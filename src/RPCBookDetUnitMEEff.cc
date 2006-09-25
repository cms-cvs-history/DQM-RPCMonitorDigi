#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <map>


//#include <DQM/RPCMonitorDigi/interface/RPCMonitorDigi.h>
#include <DQM/RPCMonitorDigi/interface/RPCMonitorEfficiency.h>
#include <DataFormats/MuonDetId/interface/RPCDetId.h>


/// Booking of MonitoringElemnt for one RPCDetId (= roll)

std::map<std::string, MonitorElement*> RPCMonitorEfficiency::bookDetUnitMEEff(RPCDetId & detId) {
 


  std::map<std::string, MonitorElement*> meMap;
 
 std::string regionName;
 std::string ringType;
 if(detId.region() ==  0) {
 	regionName="Barrel";
 	ringType="Wheel";
 }else{
        ringType="Disk";
 	if(detId.region() == -1) regionName="Encap-";
 	if(detId.region() ==  1) regionName="Encap+";
 }
 
 char  folder[120];
 sprintf(folder,"%s/%s_%d/station_%d/sector_%d",regionName.c_str(),ringType.c_str(),
 				detId.ring(),detId.station(),detId.sector());
 
  std::cout<<"SONO NEL BOOKING"<<std::endl;
  dbe->setCurrentFolder(folder);
  std::cout<<"SONO NEL BOOKING 2"<<std::endl;
 /// Name components common to current RPDDetId  
 char detUnitLabel[128];
 char layerLabel[128];
 sprintf(detUnitLabel ,"%d",detId());
 sprintf(layerLabel ,"layer%d_subsector%d_roll%d",detId.layer(),detId.subsector(),detId.roll());
  std::cout<<"SONO NEL BOOKING 3"<<std::endl;

 char meId [128];
 char meTitle [128];
  
 /// BEgin booking
 sprintf(meId,"Occupancy_%s",detUnitLabel);
 sprintf(meTitle,"Occupancy_for_%s",layerLabel);
 meMap[meId] = dbe->book1D(meId, meTitle, 100, 0.5, 100.5);

 sprintf(meId,"DetOccupancy_%s",detUnitLabel);
 sprintf(meTitle,"DetOccupancy_for_%s",layerLabel);
 meMap[meId] = dbe->book1D(meId, meTitle, 100, 0.5, 100.5);

  std::cout<<"SONO NEL BOOKING 4"<<std::endl;
 return meMap;
}
























