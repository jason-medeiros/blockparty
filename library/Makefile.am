AUTOMAKE_OPTIONS = subdir-objects

# set CPP flags.  If your includes are in a non-standard directory
# or aren't located in the default paths or ones defined below,
# add them with their own -I statement yourself.
AM_CPPFLAGS = -I./include/\
 -I/usr/include/mysql/\
 -I/usr/include/mariadb/\
 -I/usr/include/mozjs-31/\
 -I/usr/include/libxml2/\
 -Ixml2\
 -Ilibxml2\
 -I/usr/include/microhttpd/\
 -std=c++0x\
 -c\
 -fmessage-length=0


# setup library builders
lib_LIBRARIES = libblockparty.a
libblockparty_a_SOURCES = ./src/terminal/BP-LinuxTerminal.cc\
 ./src/buffer_analysis/BP-BufferAnalysis.cc\
 ./src/buffer_analysis/ASCII/BP-BufferAnalysisASCII.cc\
 ./src/static_data_sets/BP-StaticAsciiArt.cc\
 ./src/static_data_sets/BP-StaticBorderSetsNew.cc\
 ./src/static_data_sets/BP-StaticBorderSets.cc\
 ./src/static_data_sets/BP-StaticAsciiArtNew.cc\
 ./src/os_profiling/BP_LibstatgrabHelpers.cc\
 ./src/os_profiling/BP_OSProfiling.cc\
 ./src/garbage_collection/BP_MemoryGarbageCollector.cc\
 ./src/filesystem/BP-FilesystemRegistry.cc\
 ./src/filesystem/BP-Filesystem.cc\
 ./src/filesystem/BP-FilesystemRegistryPolicyEnforcementValidators.cc\
 ./src/filesystem/BP-Fifos.cc\
 ./src/command_control_interface/BP-CommandControlInterface.cc\
 ./src/command_control_interface/BP-CommandControlInterfacePty.cc\
 ./src/databases/sqlite/BP-DBSqliteDisplay.cc\
 ./src/databases/sqlite/BP-DBSqlite.cc\
 ./src/databases/mysql/BP-DBMySQL.cc\
 ./src/databases/mysql/BP-DBMySQLConnectionPooling.cc\
 ./src/databases/bpdb/BP-DBMessageReportingAndDisplayRoutines.cc\
 ./src/databases/bpdb/BP-DBStringFilters.cc\
 ./src/databases/bpdb/BP-DBJSON.cc\
 ./src/databases/bpdb/BP-DB.cc\
 ./src/databases/bpdb/BP-DBMySQLHelpers.cc\
 ./src/databases/bpdb/BP-DBSQLSyntaxGenerators.cc\
 ./src/utility/BP-GeneralPurposeTemplateSubstitution.cc\
 ./src/utility/BP-LinuxSignalAutoconfigurations.cc\
 ./src/utility/BP-NumberUtilities.cc\
 ./src/utility/BP-SeedPNRG.cc\
 ./src/utility/BP-StringUtilities.cc\
 ./src/utility/BP-StringFormattingSessions.cc\
 ./src/utility/BP-FileGeneralUtilities.cc\
 ./src/utility/BP-LinuxUserUtilities.cc\
 ./src/utility/BP-GenericBuffers.cc\
 ./src/utility/BP-TimeUtilities.cc\
 ./src/utility/BP-Syslogging.cc\
 ./src/utility/BP-ColorGenerationUtilities.cc\
 ./src/decoders/HTML/BP-HTMLDeconstructors.cc\
 ./src/decoders/HTML/BP-HTMLDecoder.cc\
 ./src/decoders/fuzzing/BP-FuzzUtilities.cc\
 ./src/decoders/x86ASM/BP-x86InstTable.cc\
 ./src/decoders/x86ASM/BP-x86InstDecode.cc\
 ./src/decoders/x86ASM/BP-x86OperandAnalysis.cc\
 ./src/decoders/x86ASM/BP-x86InstUtilities.cc\
 ./src/memory_profiling/BP_LibraryMemoryProfiling.cc\
 ./src/mmap/BP-MMapSlabAllocator.cc\
 ./src/mmap/BP-MMapAllocator.cc\
 ./src/blockclass/subclasses/command_line_tools/BP_CommandLineToolBase.cc\
 ./src/blockclass/subclasses/command_line_tools/nmap/BP_NMapApplicationController.cc\
 ./src/blockclass/subclasses/command_line_tools/BP_CLTRunMonitor.cc\
 ./src/blockclass/subclasses/command_line_tools/sqlmap/BP_SQLMapApplicationController.cc\
 ./src/blockclass/subclasses/BPSC_WebAssessmentDbInterface.cc\
 ./src/blockclass/subclasses/BP_StringUtils.cc\
 ./src/blockclass/subclasses/BP_ConfigParser.cc\
 ./src/blockclass/subclasses/BP_JSONUtils.cc\
 ./src/blockclass/subclasses/BP_CommandLineOptionParser.cc\
 ./src/blockclass/subclasses/BP_BaseError.cc\
 ./src/blockclass/subclasses/BP_WebFormSubmitter.cc\
 ./src/blockclass/subclasses/BP_StringFormattingSession.cc\
 ./src/blockclass/subclasses/BP_ApplicationHelpGenerator.cc\
 ./src/blockclass/BP-BlockClass.cc\
 ./src/dynamic_modules/BP-DynamicModules.cc\
 ./src/network/nmap/BP-NMAP.cc\
 ./src/network/email/BP-EmailSMTP.cc\
 ./src/network/email/BP-Email.cc\
 ./src/network/BP-Network.cc\
 ./src/network/nmapdb/BP-NmapDB.cc\
 ./src/network/Web/BP-NetworkWebUserAgents.cc\
 ./src/network/Web/BP-NetworkWeb.cc\
 ./src/network/BP-NetworkSSL.cc\
 ./src/network/tor/BP-Tor.cc\
 ./src/network/PCAP/BP-PacketCapture.cc\
 ./src/exploitation/BP-LinuxExploitation.cc\
 ./src/exploitation/Compromise/BP-Compromise.cc\
 ./src/exploitation/ModularExploitation/BuiltInExploitModules/UnitTestExploitModule/BP-ExploitModule_ut.cc\
 ./src/exploitation/ModularExploitation/BP-ModularExploitation.cc\
 ./src/exploitation/Utilities/PHP/GadgetGenerators/BP-PHPBuiltinGadgets.cc\
 ./src/exploitation/Utilities/PHP/GadgetGenerators/BP-PHPGadgetGenerators.cc\
 ./src/exploitation/Utilities/BP-Gadgets.cc\
 ./src/exploitation/WebExploitation/CommandExecution/BP-WebExploitationCommandExecution.cc\
 ./src/exploitation/WebExploitation/BP-WebExploitationTableLookups.cc\
 ./src/exploitation/WebExploitation/SQLErrCodes/BP-WebExploitMysqlErrCodes.cc\
 ./src/exploitation/WebExploitation/SQLErrCodes/BP-WebExploitPostgresErrCodes.cc\
 ./src/exploitation/WebExploitation/SQLErrCodes/BP-WebExploitMsqlErrCodes.cc\
 ./src/exploitation/WebExploitation/SQLErrCodes/BP-WebExploitODBCErrs.cc\
 ./src/exploitation/WebExploitation/SQLErrCodes/BP-WebExploitOracleErrCodes.cc\
 ./src/exploitation/WebExploitation/AppEngineErrCodes/BP-WebExploitJSPErrCodes.cc\
 ./src/exploitation/WebExploitation/AppEngineErrCodes/BP-WebExploitPHPErrCodes.cc\
 ./src/exploitation/WebExploitation/AppEngineErrCodes/BP-WebExploitASPErrCodes.cc\
 ./src/exploitation/WebExploitation/AppEngineErrCodes/BP-WebExploitDotNETErrCodes.cc\
 ./src/exploitation/WebExploitation/AppEngineErrCodes/BP-WebExploitCFMErrCodes.cc\
 ./src/exploitation/WebExploitation/BP-WebExploitTestResultAnalysis.cc\
 ./src/exploitation/WebExploitation/BP-WebExploitationExploitRequestSets.cc\
 ./src/exploitation/WebExploitation/BP-WebExploitationSql.cc\
 ./src/exploitation/WebExploitation/BlindSQLiDetection/BP-WebExploitationBlindSQLiDetection.cc\
 ./src/exploitation/WebExploitation/BP-WebExploitationXss.cc\
 ./src/exploitation/WebExploitation/BP-WebExploitation.cc\
 ./src/exploitation/WebExploitation/BP-WebExploitationUtils.cc\
 ./src/canaries/BP-Canaries.cc\
 ./src/datastructures/BP-LinkedList.cc\
 ./src/datastructures/BP-BufferStorageQueue.cc\
 ./src/datastructures/BP-BufferUtilities.cc\
 ./src/datastructures/BP-DataLists.cc\
 ./src/ELF/ELF32/BP-ELFExtraction_mem.cc\
 ./src/ELF/ELF32/BP-ELFExtraction.cc\
 ./src/ELF/ELF32/BP-ELFSections.cc\
 ./src/javascript/BP-Javascript.cc\
 ./src/javascript/builtin/user/BP-JSUserBuiltins.cc\
 ./src/javascript/builtin/dom/BP-JSDomBuiltins.cc\
 ./src/javascript/builtin/system/BP-JSSystemBuiltins.cc\
 ./src/javascript/builtin/admin/BP-JSAdminBuiltins.cc\
 ./src/javascript/JSON/BP-LibJSMN.cc\
 ./src/javascript/JSON/BP-JSON.cc\
 ./src/events/BP-Events.cc\
 ./src/events/BP-EventWorker.cc\
 ./src/ansible/BP-Ansible.cc\
 ./src/debugging/BP-DebugInfoQueue.cc\
 ./src/debugging/BP-LinuxBacktrace.cc\
 ./src/debugging/BP-LinuxDebug.cc\
 ./src/data_storage/BP-DataStorage.cc\
 ./src/memory/BP-LinuxMemMapping.cc

# install headers
blockpartyheadersdir = $(includedir)/blockparty
nobase_blockpartyheaders_HEADERS = ./include/terminal/BP-LinuxTerminal.h\
 ./include/buffer_analysis/BP-BufferAnalysis.h\
 ./include/buffer_analysis/ASCII/BP-BufferAnalysisASCII.h\
 ./include/static_data_sets/BP-StaticAsciiArt.h\
 ./include/static_data_sets/BP-StaticBorderSets.h\
 ./include/os_profiling/BP_LibstatgrabHelpers.h\
 ./include/os_profiling/BP_OSProfiling.h\
 ./include/garbage_collection/BP_MemoryGarbageCollector.h\
 ./include/filesystem/BP-Filesystem.h\
 ./include/filesystem/BP-FilesystemRegistry.h\
 ./include/filesystem/BP-Fifos.h\
 ./include/command_control_interface/BP-CommandControlInterface.h\
 ./include/webchat/BP-WebChat.h\
 ./include/databases/sqlite/BP-DBSqlite.h\
 ./include/databases/mysql/BP-DBMySQL.h\
 ./include/databases/mysql/BP-DBMySQLConnectionPooling.h\
 ./include/databases/bpdb/BP-DBMySQLHelpers.h\
 ./include/databases/bpdb/BP-DBSQLSyntaxGenerators.h\
 ./include/databases/bpdb/BP-DBMessageReportingAndDisplayRoutines.h\
 ./include/databases/bpdb/query_helpers/BP-DBUpdateQueryHelpers.h\
 ./include/databases/bpdb/query_helpers/BP-DBSelectQueryHelpers.h\
 ./include/databases/bpdb/query_helpers/BP-DBReplaceQueryHelpers.h\
 ./include/databases/bpdb/query_helpers/BP-DBStoredProcedureQueryHelpers.h\
 ./include/databases/bpdb/query_helpers/BP-DBInsertQueryHelpers.h\
 ./include/databases/bpdb/BP-DB.h\
 ./include/databases/bpdb/schema/results/BP-DBResultsSchema.h\
 ./include/databases/bpdb/schema/html/BP-DBHtmlSchema.h\
 ./include/databases/bpdb/schema/testsets/BP-DBTestSetsSchema.h\
 ./include/databases/bpdb/schema/intel/BP-DBIntelSchema.h\
 ./include/databases/bpdb/schema/third_party_tools/BP-DBThirdPartyToolsSchema.h\
 ./include/databases/bpdb/schema/security/BP-DBSecuritySchema.h\
 ./include/databases/bpdb/schema/assessment/BP-DBAssessmentSchema.h\
 ./include/databases/bpdb/schema/reports/BP-DBReportsSchema.h\
 ./include/databases/bpdb/schema/accesslists/BP-DBAccessListsSchema.h\
 ./include/databases/bpdb/schema/sensor/BP-DBSensorSchema.h\
 ./include/databases/bpdb/schema/datasets/BP-DBDatasetsSchema.h\
 ./include/databases/bpdb/schema/auth/BP-DBAuthSchema.h\
 ./include/databases/bpdb/BP-DBStringFilters.h\
 ./include/databases/BP-ResultDBStructuresAndTypes.h\
 ./include/r-attach/BP-TapPlugin.h\
 ./include/utility/BP-ColorGenerationUtilities.h\
 ./include/utility/BP-TimeUtilities.h\
 ./include/utility/BP-GenericFastWeightedBalancer.h\
 ./include/utility/BP-SeedPNRG.h\
 ./include/utility/BP-StringFormattingSessions.h\
 ./include/utility/BP-NumberUtilities.h\
 ./include/utility/BP-LinuxUserUtilities.h\
 ./include/utility/BP-LinuxSignalAutoconfigurations.h\
 ./include/utility/BP-GeneralPurposeTemplateSubstitution.h\
 ./include/utility/BP-StringUtilities.h\
 ./include/utility/BP-FileGeneralUtilities.h\
 ./include/utility/BP-Syslogging.h\
 ./include/utility/BP-GenericBuffers.h\
 ./include/utility/BP-ASCIICharts.h\
 ./include/errors/BP-Errors.h\
 ./include/decoders/HTML/BP-HTMLDecoder.h\
 ./include/decoders/x86ASM/BP-x86InstTable.h\
 ./include/decoders/x86ASM/BP-x86InstDecode.h\
 ./include/decoders/x86-64/BP-x86-64InstDecode.h\
 ./include/threads/BP-LinuxThreads.h\
 ./include/mmap/BP-MMapSlabAllocator.h\
 ./include/mmap/BP-MMapAllocator.h\
 ./include/semaphores/BP-Semaphores.h\
 ./include/blockclass/BP-BlockClass.h\
 ./include/blockclass/subclasses/command_line_tools/nmap/BP_NMapApplicationController.h\
 ./include/blockclass/subclasses/command_line_tools/BP_CLTRunMonitor.h\
 ./include/blockclass/subclasses/command_line_tools/sqlmap/BP_SQLMapApplicationController.h\
 ./include/blockclass/subclasses/command_line_tools/BP_CommandLineToolBase.h\
 ./include/blockclass/subclasses/BP_CommandLineOptionParser.h\
 ./include/blockclass/subclasses/BPSC_WebAssessmentDbInterface.h\
 ./include/blockclass/subclasses/BP_StringUtils.h\
 ./include/blockclass/subclasses/BP_ForkDispatcher.h\
 ./include/blockclass/subclasses/BPSC_SSHIntegrityChecker.h\
 ./include/blockclass/subclasses/BP_WebFormSubmitter.h\
 ./include/blockclass/subclasses/BP_StringFormattingSession.h\
 ./include/blockclass/subclasses/BP_ApplicationHelpGenerator.h\
 ./include/blockclass/subclasses/BP_ThreadDispatcher.h\
 ./include/blockclass/subclasses/BP_BaseError.h\
 ./include/blockclass/subclasses/BP_JSONUtils.h\
 ./include/blockclass/subclasses/BP_ThreadedWebResultStackSpider.h\
 ./include/blockclass/subclasses/BP_MySQLMultiThreadConnectionPool.h\
 ./include/blockclass/subclasses/BP_ConfigParser.h\
 ./include/blockclass/BP-BufferStorageQueue.h\
 ./include/blockclass/BP-BufferUtilities.h\
 ./include/blockclass/BP-DataLists.h\
 ./include/BP-Main.h\
 ./include/fuzzing/BP-FuzzUtilities.h\
 ./include/dynamic_modules/BP-DynamicModules.h\
 ./include/network/nmap/BP-NMAP.h\
 ./include/network/searchengines/BP-DuckDuckGoScraper.h\
 ./include/network/searchengines/BP-GoogleScraper.h\
 ./include/network/searchengines/BP-DisconnectMeScraper.h\
 ./include/network/searchengines/BP-YahooScraper.h\
 ./include/network/searchengines/BP-BingScraper.h\
 ./include/network/email/BP-Email.h\
 ./include/network/email/BP-EmailSMTP.h\
 ./include/network/Proxies/BP-NetworkProxies.h\
 ./include/network/Proxies/BP-SOCKS4Proxies.h\
 ./include/network/nmapdb/BP-NmapDB.h\
 ./include/network/httpd/promises/BP-HTTPdPromises.h\
 ./include/network/httpd/BP-HTTPdRequestHandlerHelpers.h\
 ./include/network/httpd/BP-HTTPdHelperMacros.h\
 ./include/network/httpd/BP-HTTPd.h\
 ./include/network/tor/hidden_services/BP-TorHiddenServices.h\
 ./include/network/tor/BP-Tor.h\
 ./include/network/PCAP/BP-PacketCapture.h\
 ./include/network/BP-Network.h\
 ./include/network/web/BP-NetworkWeb.h\
 ./include/network/BP-NetworkSSL.h\
 ./include/exploitation/Compromise/BP-Compromise.h\
 ./include/exploitation/ModularExploitation/BP-ModularExploitation.h\
 ./include/exploitation/ModularExploitation/BuiltInExploitModules/UnitTestExploitModule/BP-ExploitModule_ut.h\
 ./include/exploitation/Utilities/PHP/GadgetGenerators/BP-PHPGadgetGenerators.h\
 ./include/exploitation/Utilities/BP-Gadgets.h\
 ./include/exploitation/BP-LinuxExploitation.h\
 ./include/exploitation/WebExploitation/BP-WebExploitationUtils.h\
 ./include/exploitation/WebExploitation/BP-WebExploitationSql.h\
 ./include/exploitation/WebExploitation/CommandExecution/BP-WebExploitationCommandExecution.h\
 ./include/exploitation/WebExploitation/SQLErrCodes/BP-WebExploitODBCErrCodes.h\
 ./include/exploitation/WebExploitation/SQLErrCodes/BP-WebExploitPostgresErrCodes.h\
 ./include/exploitation/WebExploitation/SQLErrCodes/BP-WebExploitMsqlErrCodes.h\
 ./include/exploitation/WebExploitation/SQLErrCodes/BP-WebExploitMysqlErrCodes.h\
 ./include/exploitation/WebExploitation/SQLErrCodes/BP-WebExploitOracleErrCodes.h\
 ./include/exploitation/WebExploitation/BP-WebExploitationAppServers.h\
 ./include/exploitation/WebExploitation/BP-WebExploitationXss.h\
 ./include/exploitation/WebExploitation/BP-WebExploitationHttpErrCodes.h\
 ./include/exploitation/WebExploitation/AppEngineErrCodes/BP-WebExploitASPErrCodes.h\
 ./include/exploitation/WebExploitation/AppEngineErrCodes/BP-WebExploitPHPErrCodes.h\
 ./include/exploitation/WebExploitation/AppEngineErrCodes/BP-WebExploitCFMErrCodes.h\
 ./include/exploitation/WebExploitation/AppEngineErrCodes/BP-WebExploitDotNETErrCodes.h\
 ./include/exploitation/WebExploitation/AppEngineErrCodes/BP-WebExploitJSPErrCodes.h\
 ./include/exploitation/WebExploitation/BlindSQLiDetection/BP-WebExploitationBlindSQLiDetection.h\
 ./include/exploitation/WebExploitation/BP-WebExploitTestResultAnalysis.h\
 ./include/exploitation/WebExploitation/BP-WebExploitation.h\
 ./include/exploitation/WebExploitation/BP-WebExploitationExploitRequestSets.h\
 ./include/exploitation/WebExploitation/BP-WebExploitationMatching.h\
 ./include/exploitation/WebExploitation/BP-WebExploitationTableLookups.h\
 ./include/canaries/BP-Canaries.h\
 ./include/datastructures/hashtable/BP-HashTable.h\
 ./include/datastructures/hashtable/uthash.h\
 ./include/datastructures/hashtable/uthash_mmap.h\
 ./include/datastructures/BP-BufferUtilities.h\
 ./include/datastructures/BufferQueue/BP-BufferStorageQueue.h\
 ./include/datastructures/linkedlist/BP_LinkedList.h\
 ./include/datastructures/linkedlist/BP-DataLists.h\
 ./include/library_memory_profiling/BP_LibraryMemoryProfiling.h\
 ./include/ELF/ELF32/BP-ELFSections.h\
 ./include/ELF/ELF32/BP-ELFExtraction.h\
 ./include/javascript/builtin/user/BP-JSUserBuiltins.h\
 ./include/javascript/builtin/dom/BP-JSDomBuiltins.h\
 ./include/javascript/builtin/system/BP-JSSystemBuiltins.h\
 ./include/javascript/builtin/admin/BP-JSAdminBuiltins.h\
 ./include/javascript/BP-Javascript.h\
 ./include/javascript/JSON/BP-LibJSMN.h\
 ./include/javascript/JSON/BP-JSON.h\
 ./include/events/generic_event_subsystems_helpers/BP-EventDrivenCommandAndControlGenericSessions.h\
 ./include/events/BP-Events.h\
 ./include/events/BP-EventDrivenSubsystemHelperMacros.h\
 ./include/ansible/BP-Ansible.h\
 ./include/debugging/BP-LinuxDebugStructures.h\
 ./include/debugging/BP-DebugInfoQueue.h\
 ./include/debugging/BP-LinuxBacktrace.h\
 ./include/debugging/BP-LinuxDebug.h\
 ./include/data_storage/BP-DataStorage.h\
 ./include/memory/BP-LinuxMemMapping.h
