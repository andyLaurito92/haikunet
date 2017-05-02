// Debug global level (how much it is written to the pdevs_runX.log)  for all models unless overridden 
debugLevel = LOG_LEVEL_ALL;

// Log global level (how much it is written to scilab) for all models unless overridden
logLevel= LOG_LEVEL_NONE;

//// debug level format: 
////          - [modelName].debugLevel = [debugLevel]
////
////
//// Example of setting debug level for atomic models:
////             FelixServer1.debugLevel = LOG_LEVEL_INIT;  


//// logger level format: 
////         -  [modelName].logLevel = [logLevel]
////         -  [modelName].[variableName]. logLevel = [logLevel]
////
////
//// Example of setting log level for atomic models:
////      Link.logLevel = LOG_LEVEL_ALL; // not the processingTime
////      FelixServer1.sent_bits.logLevel = LOG_LEVEL_ALL;


//// loger specification format:
////                - [modelName].[variableName].logger = [loggerName]
////                  [modelName].[variableName].[loggerParam1] = [value1]
////                  [modelName].[variableName].[loggerParamN] = [valueN]
////
////
//// Example of setting loggers  for atomic models:
//      Log all (default):
//         FelixServer1.sent_bits.logger = LOGGER_ALL;
//      Sampler Logger:
//          FelixServer1.sent_bits.logger = LOGGER_SAMPLER;
//          FelixServer1.sent_bits.sample_period = samplingPeriod;

//
