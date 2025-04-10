void sim(Int_t nev=1) {
  new AliRun("gAlice","The ALICE Off-line Simulation Framework");

  AliSimulation simulator;

  simulator.SetDefaultStorage("local://$ALIROOT_OCDB_ROOT/OCDB");
  simulator.SetSpecificStorage("GRP/GRP/Data",
			       Form("local://%s",gSystem->pwd()));

  simulator.SetRunHLT("default"); // In case we do not have ancored production
  simulator.SetRunQA(":");

  TStopwatch timer;
  timer.Start();
  simulator.Run(nev);
  timer.Stop();
  timer.Print();
}
