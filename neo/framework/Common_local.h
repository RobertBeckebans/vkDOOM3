/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company.
Copyright (C) 2016-2017 Dustin Land

This file is part of the Doom 3 BFG Edition GPL Source Code ("Doom 3 BFG Edition Source Code").

Doom 3 BFG Edition Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 BFG Edition Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 BFG Edition Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 BFG Edition Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 BFG Edition Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

static const int MAX_USERCMD_BACKUP = 256;
static const int NUM_USERCMD_RELAY = 10;
static const int NUM_USERCMD_SEND = 8;

static const int initialHz = 60;
static const int initialBaseTicks = 1000 / initialHz;
static const int initialBaseTicksPerSec = initialHz * initialBaseTicks;

static const int LOAD_TIP_CHANGE_INTERVAL = 12000;
static const int LOAD_TIP_COUNT = 26;

class idGameThread : public idSysThread
{
public:
	idGameThread() :
		userCmdMgr( NULL ),
		ret(),
		numGameFrames(),
		isClient()
	{}
	
	// the gameReturn_t is from the previous frame, the
	// new frame will be running in parallel on exit
	gameReturn_t	RunGameAndDraw( int numGameFrames, idUserCmdMgr& userCmdMgr_, bool isClient_, int startGameFrame );
	
private:
	virtual int	Run();
	
	idUserCmdMgr* 	userCmdMgr;
	gameReturn_t	ret;
	int				numGameFrames;
	bool			isClient;
};

enum errorParm_t
{
	ERP_NONE,
	ERP_FATAL,						// exit the entire game with a popup window
	ERP_DROP,						// print to console and disconnect from game
	ERP_DISCONNECT					// don't kill server
};

enum gameLaunch_t
{
	LAUNCH_TITLE_DOOM = 0,
	LAUNCH_TITLE_DOOM2,
};

struct netTimes_t
{
	int localTime;
	int serverTime;
};

struct frameTiming_t
{
	frameTiming_t()
	{
		Reset();
	}
	
	void Reset()
	{
		startSyncTime		= 0;
		finishSyncTime		= 0;
		startGameTime		= 0;
		finishGameTime		= 0;
		finishDrawTime		= 0;
		startRenderTime		= 0;
		finishRenderTime	= 0;
		
		frontendTime		= 0;
		backendTime			= 0;
		depthTime			= 0;
		interactionTime		= 0;
		shaderTime			= 0;
		shadowTime			= 0;
		gpuTime				= 0;
		
		samples				= 0;
		backendTotalTime	= 0;
		gpuTotalTime		= 0;
		backendTimeAvg		= 0.0;
		gpuTimeAvg			= 0.0;
	}
	
	void Update( frameTiming_t& other )
	{
		startSyncTime		= other.startSyncTime;
		finishSyncTime		= other.finishSyncTime;
		startGameTime		= other.startGameTime;
		finishGameTime		= other.finishGameTime;
		finishDrawTime		= other.finishDrawTime;
		startRenderTime		= other.startRenderTime;
		finishRenderTime	= other.finishRenderTime;
		
		frontendTime		= other.frontendTime;
		backendTime			= other.backendTime;
		depthTime			= other.depthTime;
		interactionTime		= other.interactionTime;
		shaderTime			= other.shaderTime;
		shadowTime			= other.shadowTime;
		gpuTime				= other.gpuTime;
		
		samples++;
		backendTotalTime	+= backendTime;
		gpuTotalTime		+= gpuTime;
		backendTimeAvg		= backendTotalTime / samples;
		gpuTimeAvg			= gpuTotalTime / samples;
	}
	
	uint64	startSyncTime;
	uint64	finishSyncTime;
	uint64	startGameTime;
	uint64	finishGameTime;
	uint64	finishDrawTime;
	uint64	startRenderTime;
	uint64	finishRenderTime;
	
	uint64	frontendTime;
	uint64	backendTime;
	uint64	depthTime;
	uint64	interactionTime;
	uint64	shaderTime;
	uint64	shadowTime;
	uint64	gpuTime;
	
	uint64	samples;
	uint64	backendTotalTime;
	uint64	gpuTotalTime;
	double	backendTimeAvg;
	double	gpuTimeAvg;
};

#define	MAX_PRINT_MSG_SIZE	4096
#define MAX_WARNING_LIST	256

#define SAVEGAME_CHECKPOINT_FILENAME		"gamedata.save"
#define SAVEGAME_DESCRIPTION_FILENAME		"gamedata.txt"
#define SAVEGAME_STRINGS_FILENAME			"gamedata.strings"

class idCommonLocal : public idCommon
{
public:
	idCommonLocal();
	
	virtual void				Init( int argc, const char* const* argv, const char* cmdline );
	virtual void				Shutdown();
	virtual	void				CreateMainMenu();
	virtual void				Quit();
	virtual bool				IsInitialized() const;
	virtual void				Frame();
	virtual void				UpdateScreen();
	virtual void				UpdateLevelLoadPacifier();
	virtual void				StartupVariable( const char* match );
	virtual void				WriteConfigToFile( const char* filename );
	virtual void				BeginRedirect( char* buffer, int buffersize, void ( *flush )( const char* ) );
	virtual void				EndRedirect();
	virtual void				SetRefreshOnPrint( bool set );
	virtual void				Printf( const char* fmt, ... );
	virtual void				VPrintf( const char* fmt, va_list arg );
	virtual void				DPrintf( const char* fmt, ... );
	virtual void				Warning( const char* fmt, ... );
	virtual void				DWarning( const char* fmt, ... );
	virtual void				PrintWarnings();
	virtual void				ClearWarnings( const char* reason );
	virtual void				Error( const char* fmt, ... );
	virtual void				FatalError( const char* fmt, ... );
	virtual bool				IsShuttingDown() const
	{
		return com_shuttingDown;
	}
	
	virtual const char* 		KeysFromBinding( const char* bind );
	virtual const char* 		BindingFromKey( const char* key );
	
	virtual bool				IsMultiplayer();
	virtual bool				IsServer();
	virtual bool				IsClient();
	
	virtual bool				GetConsoleUsed()
	{
		return consoleUsed;
	}
	
	virtual int					GetSnapRate();
	
	virtual void				NetReceiveReliable( int peer, int type, idBitMsg& msg );
	virtual void				NetReceiveSnapshot( class idSnapShot& ss );
	virtual void				NetReceiveUsercmds( int peer, idBitMsg& msg );
	void						NetReadUsercmds( int clientNum, idBitMsg& msg );
	
	virtual bool				ProcessEvent( const sysEvent_t* event );
	
	virtual bool				LoadGame( const char* saveName );
	virtual bool				SaveGame( const char* saveName );
	
	virtual int					ButtonState( int key );
	virtual int					KeyState( int key );
	
	virtual idGame* 			Game()
	{
		return game;
	}
	virtual idRenderWorld* 		RW()
	{
		return renderWorld;
	}
	virtual idSoundWorld* 		SW()
	{
		return soundWorld;
	}
	virtual idSoundWorld* 		MenuSW()
	{
		return menuSoundWorld;
	}
	virtual idSession* 			Session()
	{
		return session;
	}
	virtual idCommonDialog& 	Dialog()
	{
		return commonDialog;
	}
	
	virtual void				OnSaveCompleted( idSaveLoadParms& parms );
	virtual void				OnLoadCompleted( idSaveLoadParms& parms );
	virtual void				OnLoadFilesCompleted( idSaveLoadParms& parms );
	virtual void				OnEnumerationCompleted( idSaveLoadParms& parms );
	virtual void				OnDeleteCompleted( idSaveLoadParms& parms );
	virtual void				TriggerScreenWipe( const char* _wipeMaterial, bool hold );
	
	virtual void				OnStartHosting( idMatchParameters& parms );
	
	virtual int					GetGameFrame()
	{
		return gameFrame;
	}
	
	virtual void				LaunchExternalTitle( int titleIndex,
			int device,
			const lobbyConnectInfo_t* const connectInfo );  // For handling invitations. NULL if no invitation used.
			
	virtual void				InitializeMPMapsModes();
	virtual const idStrList& 			GetModeList() const
	{
		return mpGameModes;
	}
	virtual const idStrList& 			GetModeDisplayList() const
	{
		return mpDisplayGameModes;
	}
	virtual const idList<mpMap_t>& 		GetMapList() const
	{
		return mpGameMaps;
	}
	
	virtual void				ResetPlayerInput( int playerIndex );
	
	virtual bool				JapaneseCensorship() const;
	
	virtual void				QueueShowShell()
	{
		showShellRequested = true;
	}
	
	virtual currentGame_t		GetCurrentGame() const
	{
		return currentGame;
	}
	virtual void				SwitchToGame( currentGame_t newGame );
	
public:
	void	Draw();			// called by gameThread
	
	frameTiming_t		m_frameTiming;
	frameTiming_t		m_mainFrameTiming;
	
public:	// These are public because they are called directly by static functions in this file

	const char* GetCurrentMapName()
	{
		return currentMapName.c_str();
	}
	
	// loads a map and starts a new game on it
	void	StartNewGame( const char* mapName, bool devmap, int gameMode );
	void	LeaveGame();
	
	// localization
	void	InitLanguageDict();
	void	LocalizeGui( const char* fileName, idLangDict& langDict );
	void	LocalizeMapData( const char* fileName, idLangDict& langDict );
	void	LocalizeSpecificMapData( const char* fileName, idLangDict& langDict, const idLangDict& replaceArgs );
	
	idUserCmdMgr& GetUCmdMgr()
	{
		return userCmdMgr;
	}
	
private:
	bool						com_fullyInitialized;
	bool						com_refreshOnPrint;		// update the screen every print for dmap
	errorParm_t					com_errorEntered;
	bool						com_shuttingDown;
	bool						com_isJapaneseSKU;
	
	idFile* 					logFile;
	
	char						errorMessage[MAX_PRINT_MSG_SIZE];
	
	char* 						rd_buffer;
	int							rd_buffersize;
	void	( *rd_flush )( const char* buffer );
	
	idStr						warningCaption;
	idStrList					warningList;
	idStrList					errorList;
	
	int							gameDLL;
	
	idCommonDialog				commonDialog;
	
	idFile_SaveGame 			saveFile;
	idFile_SaveGame 			stringsFile;
	idFile_SaveGamePipelined*	 pipelineFile;
	
	// The main render world and sound world
	idRenderWorld* 		renderWorld;
	idSoundWorld* 		soundWorld;
	
	bool				menuActive;
	idSoundWorld* 		menuSoundWorld;			// so the game soundWorld can be muted
	
	bool				insideExecuteMapChange;	// Enable Pacifier Updates
	
	// This is set if the player enables the console, which disables achievements
	bool				consoleUsed;
	
	// This additional information is required for ExecuteMapChange for SP games ONLY
	// This data is cleared after ExecuteMapChange
	struct mapSpawnData_t
	{
		idFile_SaveGame* 	savegameFile;				// Used for loading a save game
		idFile_SaveGame* 	stringTableFile;			// String table read from save game loaded
		idFile_SaveGamePipelined* pipelineFile;
		int					savegameVersion;			// Version of the save game we're loading
		idDict				persistentPlayerInfo;		// Used for transitioning from map to map
	};
	mapSpawnData_t		mapSpawnData;
	idStr				currentMapName;			// for checking reload on same level
	bool				mapSpawned;				// cleared on Stop()
	
	bool				insideUpdateScreen;		// true while inside ::UpdateScreen()
	
	idUserCmdMgr		userCmdMgr;
	
	int					nextUsercmdSendTime;	// Next time to send usercmds
	int					nextSnapshotSendTime;	// Next time to send a snapshot
	
	idSnapShot			lastSnapShot;		// last snapshot we received from the server
	struct reliableMsg_t
	{
		int	client;
		int type;
		int dataSize;
		byte* data;
	};
	idList<reliableMsg_t> reliableQueue;
	
	
	// Snapshot interpolation
	idSnapShot		oldss;				// last local snapshot
	// (ie on server this is the last "master" snapshot  we created)
	// (on clients this is the last received snapshot)
	// used for comparisons with the new snapshot for com_drawSnapshot
	
	// This is ultimately controlled by net_maxBufferedSnapshots by running double speed, but this is the hard max before seeing visual popping
	static const int RECEIVE_SNAPSHOT_BUFFER_SIZE = 16;
	
	int				readSnapshotIndex;
	int				writeSnapshotIndex;
	idArray<idSnapShot, RECEIVE_SNAPSHOT_BUFFER_SIZE>	receivedSnaps;
	
	float			optimalPCTBuffer;
	float			optimalTimeBuffered;
	float			optimalTimeBufferedWindow;
	
	uint64			snapRate;
	uint64			actualRate;
	
	uint64			snapTime;			// time we got the most recent snapshot
	uint64			snapTimeDelta;		// time interval that current ss was sent in
	
	uint64			snapTimeWrite;
	uint64			snapCurrentTime;	// realtime playback time
	netTimes_t		snapCurrent;		// current snapshot
	netTimes_t		snapPrevious;		// previous snapshot
	float			snapCurrentResidual;
	
	float			snapTimeBuffered;
	float			effectiveSnapRate;
	int				totalBufferedTime;
	int				totalRecvTime;
	
	int					clientPrediction;
	
	int					gameFrame;			// Frame number of the local game
	double				gameTimeResidual;	// left over msec from the last game frame
	bool				syncNextGameFrame;
	
	enum timeDemo_t
	{
		TD_NO,
		TD_YES,
		TD_YES_THEN_QUIT
	};
	timeDemo_t			timeDemo;
	int					timeDemoStartTime;
	int					numDemoFrames;		// for timeDemo and demoShot
	int					demoTimeOffset;
	renderView_t		currentDemoRenderView;
	
	idStrList			mpGameModes;
	idStrList			mpDisplayGameModes;
	idList<mpMap_t>		mpGameMaps;
	
	idSWF* 				loadGUI;
	int					nextLoadTip;
	bool				isHellMap;
	bool				defaultLoadscreen;
	idStaticList<int, LOAD_TIP_COUNT>	loadTipList;
	
	const idMaterial* 	splashScreen;
	
	const idMaterial* 	whiteMaterial;
	
	const idMaterial* 	wipeMaterial;
	int					wipeStartTime;
	int					wipeStopTime;
	bool				wipeHold;
	bool				wipeForced;		// used for the PS3 to start an early wipe while we are accessing saved game data
	
	idGameThread		gameThread;				// the game and draw code can be run in parallel
	
	// Used during loading screens
	int					lastPacifierSessionTime;
	int					lastPacifierGuiTime;
	bool				lastPacifierDialogState;
	
	bool				showShellRequested;
	
	currentGame_t		currentGame;
	currentGame_t		idealCurrentGame;		// Defer game switching so that bad things don't happen in the middle of the frame.
	const idMaterial* 	doomClassicMaterial;
	
	static const int			DOOMCLASSIC_RENDERWIDTH = 320 * 3;
	static const int			DOOMCLASSIC_RENDERHEIGHT = 200 * 3;
	static const int			DOOMCLASSIC_BYTES_PER_PIXEL = 4;
	static const int			DOOMCLASSIC_IMAGE_SIZE_IN_BYTES = DOOMCLASSIC_RENDERWIDTH * DOOMCLASSIC_RENDERHEIGHT * DOOMCLASSIC_BYTES_PER_PIXEL;
	
	idArray< byte, DOOMCLASSIC_IMAGE_SIZE_IN_BYTES >	doomClassicImageData;
	
private:
	void	InitCommands();
	void	InitSIMD();
	void	AddStartupCommands();
	void	ParseCommandLine( int argc, const char* const* argv );
	bool	SafeMode();
	void	CloseLogFile();
	void	WriteConfiguration();
	void	DumpWarnings();
	void	LoadGameDLL();
	void	UnloadGameDLL();
	void	CleanupShell();
	void	RenderBink( const char* path );
	void	RenderSplash();
	void	FilterLangList( idStrList* list, idStr lang );
	void	CheckStartupStorageRequirements();
	
	void	ExitMenu();
	bool	MenuEvent( const sysEvent_t* event );
	
	void	StartMenu( bool playIntro = false );
	void	GuiFrameEvents();
	
	void	AdvanceRenderDemo( bool singleFrameOnly );
	
	void	ProcessGameReturn( const gameReturn_t& ret );
	
	void	RunNetworkSnapshotFrame();
	void	ExecuteReliableMessages();
	
	
	// Snapshot interpolation
	void	ProcessSnapshot( idSnapShot& ss );
	int		CalcSnapTimeBuffered( int& totalBufferedTime, int& totalRecvTime );
	void	ProcessNextSnapshot();
	void	InterpolateSnapshot( netTimes_t& prev, netTimes_t& next, float fraction, bool predict );
	void	ResetNetworkingState();
	
	int		NetworkFrame();
	void	SendSnapshots();
	void	SendUsercmds( int localClientNum );
	
	void	LoadLoadingGui( const char* mapName, bool& hellMap );
	
	// Meant to be used like:
	// while ( waiting ) { BusyWait(); }
	void	BusyWait();
	bool	WaitForSessionState( idSession::sessionState_t desiredState );
	
	void	ExecuteMapChange();
	void	UnloadMap();
	
	void	Stop( bool resetSession = true );
	
	// called by Draw when the scene to scene wipe is still running
	void	DrawWipeModel();
	void	StartWipe( const char* materialName, bool hold = false );
	void	CompleteWipe();
	void	ClearWipe();
	
	void	MoveToNewMap( const char* mapName, bool devmap );
	
	void	PlayIntroGui();
	
	void	ScrubSaveGameFileName( idStr& saveFileName ) const;
	
	// Doom classic support
	void	RunDoomClassicFrame();
	void	RenderDoomClassic();
	bool	IsPlayingDoomClassic() const
	{
		return GetCurrentGame() != DOOM3_BFG;
	}
	void	PerformGameSwitch();
};

extern idCommonLocal commonLocal;