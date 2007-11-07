std::cout <<"\t Getting the DT Geometry"<<std::endl;
edm::ESHandle<DTGeometry> dtGeo;
iSetup.get<MuonGeometryRecord>().get(dtGeo);
    
std::cout <<"\t Getting the DT Segments"<<std::endl;
edm::Handle<DTRecSegment4DCollection> all4DSegments;
iEvent.getByLabel(dt4DSegments, all4DSegments);

if(all4DSegments->size()>0){
  std::cout<<"\t Number of Segments in this event = "<<all4DSegments->size()<<std::endl;
  
  std::map<DTChamberId,int> scounter;
  DTRecSegment4DCollection::const_iterator segment;  
  
  for (segment = all4DSegments->begin();segment!=all4DSegments->end(); ++segment){
    scounter[segment->chamberId()]++;
  }    
  
  std::cout<<"\t Loop over all the 4D Segments"<<std::endl;
  for (segment = all4DSegments->begin(); segment != all4DSegments->end(); ++segment){ 
    
    DTChamberId DTId = segment->chamberId();
    
    std::cout<<"\t \t This Segment is in Chamber id: "<<DTId<<std::endl;
    std::cout<<"\t \t Number of segments in this DT = "<<scounter[DTId]<<std::endl;
    std::cout<<"\t \t Is the only one in this DT?"<<std::endl;

    if(scounter[DTId]==1 && DTId.station()!=4){	
      std::cout<<"\t \t yes"<<std::endl;
      int dtWheel = DTId.wheel();
      int dtStation = DTId.station();
      int dtSector = DTId.sector();
      
      LocalPoint segmentPosition= segment->localPosition();
      LocalVector segmentDirection=segment->localDirection();
      
      const GeomDet* gdet=dtGeo->idToDet(segment->geographicalId());
      const BoundPlane & DTSurface = gdet->surface();
      
      //check if the dimension of the segment is 4 
      std::cout<<"\t \t Is the segment 4D?"<<std::endl;
      
      if(segment->dimension()==4){
	std::cout<<"\t \t yes"<<std::endl;
	std::cout<<"\t \t DT Segment Dimension "<<segment->dimension()<<std::endl; 

	float Xo=segmentPosition.x();
	float Yo=segmentPosition.y();
	float dx=segmentDirection.x();
	float dy=segmentDirection.y();
	float dz=segmentDirection.z();
	std::cout<<"\t \t Loop over all the rolls asociated to this DT"<<std::endl;

	std::set<RPCDetId> rollsForThisDT = rollstoreDT[DTStationIndex(0,dtWheel,dtSector,dtStation)];
	
	std::cout<<"\t \t Number of rolls for this DT = "<<rollsForThisDT.size()<<std::endl;
        assert(rollsForThisDT.size()>=1);

	//Loop over all the rolls
	
	for (std::set<RPCDetId>::iterator iteraRoll = rollsForThisDT.begin();iteraRoll != rollsForThisDT.end(); iteraRoll++){
	  const RPCRoll* rollasociated = rpcGeo->roll(*iteraRoll);
	  const BoundPlane & RPCSurface = rollasociated->surface(); 

	  std::cout<<"\t \t \t RollID: "<<rollasociated->id()<<std::endl;
	  std::cout<<"\t \t \t Doing the extrapolation to this roll"<<std::endl;

	  std::cout<<"\t \t \t DT Segment Direction in DTLocal "<<segmentDirection<<std::endl;
	  std::cout<<"\t \t \t DT Segment Point in DTLocal "<<segmentPosition<<std::endl;
	  
	  GlobalPoint CenterPointRollGlobal = RPCSurface.toGlobal(LocalPoint(0,0,0));
	  std::cout<<"\t \t \t Center (0,0,0) of the Roll in Global"<<CenterPointRollGlobal<<std::endl;
	    
	  LocalPoint CenterRollinDTFrame = DTSurface.toLocal(CenterPointRollGlobal);
	  std::cout<<"\t \t \t Center (0,0,0) Roll In DTLocal"<<CenterRollinDTFrame<<std::endl;
	    
	  float D=CenterRollinDTFrame.z();
	  
	    float X=Xo+dx*D/dz;
	    float Y=Yo+dy*D/dz;
	    float Z=D;
	
	  std::cout<<"\t \t \t Is the distance less than MaxD? D="<<D<<"cm"<<std::endl;
  

	  if(X*X+Y*Y+Z*Z<=MaxD*MaxD){ 
	    std::cout<<"\t \t \t yes"<<std::endl;
	    const RectangularStripTopology* top_= dynamic_cast<const RectangularStripTopology*> (&(rollasociated->topology()));
	    LocalPoint xmin = top_->localPosition(0.);
	    LocalPoint xmax = top_->localPosition((float)rollasociated->nstrips());
	    float rsize = fabs( xmax.x()-xmin.x() )*0.5;
	    float stripl = top_->stripLength();
	    
	    std::cout<<"\t \t \t X Predicted in DTLocal= "<<X<<"cm"<<std::endl;
	    std::cout<<"\t \t \t Y Predicted in DTLocal= "<<Y<<"cm"<<std::endl;
	    std::cout<<"\t \t \t Z Predicted in DTLocal= "<<Z<<"cm"<<std::endl;
	    
	    GlobalPoint GlobalPointExtrapolated = DTSurface.toGlobal(LocalPoint(X,Y,Z));
	    std::cout<<"\t \t \t Point ExtraPolated in Global"<<GlobalPointExtrapolated<< std::endl;
	    
	    LocalPoint PointExtrapolatedRPCFrame = RPCSurface.toLocal(GlobalPointExtrapolated);
	    std::cout<<"\t \t \t Point Extrapolated in RPCLocal"<<PointExtrapolatedRPCFrame<< std::endl;
	    std::cout<<"\t \t \t Does the extrapolation go inside this roll?"<<std::endl;
	    
	    if(fabs(PointExtrapolatedRPCFrame.z()) < 0.01 && fabs(PointExtrapolatedRPCFrame.x()) < rsize && fabs(PointExtrapolatedRPCFrame.y()) < stripl*0.5){
	      
	      RPCDetId  rollId = rollasociated->id();
	      std::cout<<"\t \t \t \t yes"<<std::endl;	
	      const float stripPredicted = 
		rollasociated->strip(LocalPoint(PointExtrapolatedRPCFrame.x(),PointExtrapolatedRPCFrame.y(),0.)); 
	      
	      std::cout<<"\t \t \t \t Candidate"<<rollasociated->id()<<" "<<"(from DT Segment) STRIP---> "<<stripPredicted<< std::endl;
	      
	      int stripDetected = 0;
	      RPCDigiCollection::Range rpcRangeDigi=rpcDigis->get(rollasociated->id());
	      
	      int stripCounter = 0;


	      //--------- HISTOGRAM STRIP PREDICTED FROM DT  -------------------
	      
	      uint32_t id = rollId.rawId();
	      _idList.push_back(id);
	      
	      char detUnitLabel[128];
	      sprintf(detUnitLabel ,"%d",id);
	      sprintf(layerLabel ,"layer%d_subsector%d_roll%d",rollId.layer(),rollId.subsector(),rollId.roll());
	      
	      std::map<uint32_t, std::map<std::string,MonitorElement*> >::iterator meItr = meCollection.find(id);
	      if (meItr == meCollection.end()){
		meCollection[id] = bookDetUnitSeg(rollId);
	      }
	      
	      std::map<std::string, MonitorElement*> meMap=meCollection[id];

	      sprintf(meIdDT,"ExpectedOccupancyFromDT_%s",detUnitLabel);
	      meMap[meIdDT]->Fill(stripPredicted);

	      sprintf(meIdDT,"ExpectedOccupancy2DFromDT_%s",detUnitLabel);
	      meMap[meIdDT]->Fill(stripPredicted,Y);

	      //-----------------------------------------------------
	    
	      totalcounter[0]++;
	      buff=counter[0];
	      buff[rollasociated->id()]++;
	      counter[0]=buff;
	      
	      bool anycoincidence=false;

	      
	      std::cout<<"\t \t \t \t \t Loop over the digis in this roll "<<std::endl;

	      for (RPCDigiCollection::const_iterator digiIt = rpcRangeDigi.first;digiIt!=rpcRangeDigi.second;++digiIt){
		stripCounter++;
		std::cout<<"\t \t \t \t \t Digi "<<*digiIt<<std::endl;//print the digis in the event
		stripDetected=digiIt->strip();
		
		double res = fabs((double)(stripDetected) - (double)(stripPredicted));

		//-------filling the histograms--------------------

		sprintf(meIdRPC,"RPCResidualsFromDT_%s",detUnitLabel);
		meMap[meIdRPC]->Fill(res);
		
		sprintf(meIdRPC,"RPCResiduals2DFromDT_%s",detUnitLabel);
		meMap[meIdRPC]->Fill(res,Y);


		//-------------------------------------------------

		
		if(res < widestrip){
		  std::cout <<"\t \t \t \t \t COINCEDENCE Predict "<<stripPredicted<<" Detect "<<stripDetected<<std::endl;
		  anycoincidence=true;
		  std::cout <<"\t \t \t \t \t Increassing counter"<<std::endl;
		  totalcounter[1]++;
		  buff=counter[1];
		  buff[rollId]++;
		  counter[1]=buff;

		  //----------------filling the histograms---------------------

		  sprintf(meIdRPC,"RealDetectedOccupancyFromDT_%s",detUnitLabel);
		  meMap[meIdRPC]->Fill(stripDetected);

		  sprintf(meIdRPC,"RPCDataOccupancyFromDT_%s",detUnitLabel);
		  meMap[meIdRPC]->Fill(stripPredicted);
		
		  sprintf(meIdRPC,"RPCDataOccupancy2DFromDT_%s",detUnitLabel);
		  meMap[meIdRPC]->Fill(stripPredicted,Y);
		
		  //-----------------------------------------------------------

		  break;
		}
	      }
	      if(anycoincidence==false) {
		std::cout <<"\t \t \t \t \t THIS PREDICTION DOESN'T HAVE ANY CORRESPONDENCE WITH THE DATA"<<std::endl;
		totalcounter[2]++;
		buff=counter[2];
		buff[rollId]++;
		counter[2]=buff;		
		std::cout << "\t \t \t \t \t One for counterFAIL"<<std::endl;
		
		ofrej<<"DTs Wh "<<dtWheel
		     <<"\t St "<<dtStation
		     <<"\t Se "<<dtSector
		     <<"\t Roll "<<rollasociated->id()
		     <<"\t Event "
		     <<iEvent.id().event()
		     <<std::endl;
	      }
	      
	    }
	    else {
	      std::cout<<"\t \t \t \t No the prediction is outside of this roll"<<std::endl;
	    }//Condition for the right match
	  }else{
	    std::cout<<"\t \t \t No, Exrtrapolation too long!, canceled"<<std::endl;
	  }//D so big
	}//loop over all the rolls
      }
    }
    else {
      std::cout<<"\t \t No More than one segment in this chamber."<<std::endl;
    }
  }
}
else {
  std::cout<<"This Event doesn't have any DT4DDSegment"<<std::endl; //is ther more than 1 segment in this event?
}
