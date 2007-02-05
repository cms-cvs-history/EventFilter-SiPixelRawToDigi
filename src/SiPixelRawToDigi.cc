#include "EventFilter/SiPixelRawToDigi/interface/SiPixelRawToDigi.h"

#include "FWCore/Framework/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/SiPixelDigi/interface/PixelDigi.h"
#include "DataFormats/SiPixelDigi/interface/PixelDigiCollection.h"

#include "DataFormats/DetId/interface/DetId.h"

#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"
#include "DataFormats/FEDRawData/interface/FEDRawData.h"

#include "DataFormats/FEDRawData/interface/FEDNumbering.h"

#include "CondFormats/SiPixelObjects/interface/SiPixelFedCablingMap.h"
#include "CondFormats/DataRecord/interface/SiPixelFedCablingMapRcd.h"

#include "EventFilter/SiPixelRawToDigi/interface/PixelDataFormatter.h"

#include "R2DTimerObserver.h"

#include "TH1D.h"
#include "TFile.h"
#include "TROOT.h"

using namespace std;

// -----------------------------------------------------------------------------
SiPixelRawToDigi::SiPixelRawToDigi( const edm::ParameterSet& conf ) 
  : eventCounter_(0), 
    theLabel( conf.getParameter<edm::InputTag>( "src") ),
    fedCablingMap_(0)
{
  edm::LogInfo("SiPixelRawToDigi")<< " HERE ** constructor!" << endl;
  produces< edm::DetSetVector<PixelDigi> >();

  rootFile = new TFile("analysis.root", "RECREATE", "my histograms");
  hCPU = new TH1D ("hCPU","hCPU",60,0.,0.030);
  hDigi = new TH1D("hDigi","hDigi",50,0.,15000.);
}


// -----------------------------------------------------------------------------
SiPixelRawToDigi::~SiPixelRawToDigi() {
  edm::LogInfo("SiPixelRawToDigi")  << " HERE ** SiPixelRawToDigi destructor!";
  rootFile->Write();
  cout << " end.."<<endl;
}


// -----------------------------------------------------------------------------
void SiPixelRawToDigi::beginJob(const edm::EventSetup& c) 
{
}

// -----------------------------------------------------------------------------
void SiPixelRawToDigi::produce( edm::Event& ev,
                              const edm::EventSetup& es) 
{

  edm::ESHandle<SiPixelFedCablingMap> map;
  es.get<SiPixelFedCablingMapRcd>().get( map );
  cout << map->version() << endl;

  static  R2DTimerObserver timer("**** MY TIMING REPORT ***");

  edm::Handle<FEDRawDataCollection> buffers;
  ev.getByLabel(theLabel, buffers);


// create product (digis)
  std::auto_ptr< edm::DetSetVector<PixelDigi> > collection( new edm::DetSetVector<PixelDigi> );
  static int ndigis = 0;
  static int nwords = 0;

  PixelDataFormatter formatter(map.product());
{
  TimeMe t(timer.item(), false);
  FEDNumbering fednum;
  pair<int,int> fedIds = fednum.getSiPixelFEDIds();
  fedIds.first = 1;
  fedIds.second = 40; //  temporary FIX !!!!
  

  for (int fedId = fedIds.first; fedId <= fedIds.second; fedId++) {
    LogDebug("SiPixelRawToDigi")<< " PRODUCE DIGI FOR FED: " <<  fedId << endl;
    PixelDataFormatter::Digis digis;
     
    //get event data for this fed
    const FEDRawData& fedRawData = buffers->FEDData( fedId );

    //convert data to digi
    formatter.interpretRawData( fedId, fedRawData, digis);

    //pack digi into collection
    typedef PixelDataFormatter::Digis::iterator ID;
    for (ID it = digis.begin(); it != digis.end(); it++) {
      uint32_t detid = it->id;
      edm::DetSet<PixelDigi>& detSet = collection->find_or_insert(detid);
      detSet.data = it->data;
    } 
  }
}
  cout << "TIMING IS: (real)" << timer.lastMeasurement().real() << endl;
  ndigis += formatter.nDigis();
  nwords += formatter.nWords();
  cout << " (Words/Digis) this ev: "<<formatter.nWords()<<"/"<<formatter.nDigis()
       << "--- all :"<<nwords<<"/"<<ndigis<<endl;
  hCPU->Fill( timer.lastMeasurement().real() ); 
  hDigi->Fill(formatter.nDigis());

  //send digis back to framework 
  ev.put( collection );
}

