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

#ifndef __RENDERER_BACKEND_H__
#define __RENDERER_BACKEND_H__

struct tmu_t
{
	unsigned int	current2DMap;
	unsigned int	currentCubeMap;
};

const int MAX_MULTITEXTURE_UNITS =	8;

enum stencilFace_t
{
	STENCIL_FACE_FRONT,
	STENCIL_FACE_BACK,
	STENCIL_FACE_NUM
};

struct backEndCounters_t
{
	int		c_surfaces;
	int		c_shaders;

	int		c_drawElements;
	int		c_drawIndexes;

	int		c_shadowElements;
	int		c_shadowIndexes;

	int		c_copyFrameBuffer;

	float	c_overDraw;

	int		totalMicroSec;		// total microseconds for backend run
	int		shadowMicroSec;
};

struct gfxImpParms_t
{
	int		x;				// ignored in fullscreen
	int		y;				// ignored in fullscreen
	int		width;
	int		height;
	int		fullScreen;		// 0 = windowed, otherwise 1 based monitor number to go full screen on
	// -1 = borderless window for spanning multiple displays
	int		displayHz;
	int		multiSamples;
};

#define MAX_DEBUG_LINES		16384
#define MAX_DEBUG_TEXT		512
#define MAX_DEBUG_POLYGONS	8192

struct debugLine_t
{
	idVec4		rgb;
	idVec3		start;
	idVec3		end;
	bool		depthTest;
	int			lifeTime;
};

struct debugText_t
{
	idStr		text;
	idVec3		origin;
	float		scale;
	idVec4		color;
	idMat3		viewAxis;
	int			align;
	int			lifeTime;
	bool		depthTest;
};

struct debugPolygon_t
{
	idVec4		rgb;
	idWinding	winding;
	bool		depthTest;
	int			lifeTime;
};

void RB_SetMVP( const idRenderMatrix& mvp );
void RB_SetVertexColorParms( stageVertexColor_t svc );
void RB_GetShaderTextureMatrix( const float* shaderRegisters, const textureStage_t* texture, float matrix[16] );
void RB_LoadShaderTextureMatrix( const float* shaderRegisters, const textureStage_t* texture );
void RB_BakeTextureMatrixIntoTexgen( idPlane lightProject[3], const float* textureMatrix );
void RB_SetupInteractionStage( const shaderStage_t* surfaceStage, const float* surfaceRegs, const float lightColor[4], idVec4 matrix[2], float color[4] );

bool ChangeDisplaySettingsIfNeeded( gfxImpParms_t parms );
bool CreateGameWindow( gfxImpParms_t parms );

#if defined( ID_VULKAN )

struct gpuInfo_t
{
	VkPhysicalDevice					device;
	VkPhysicalDeviceProperties			props;
	VkPhysicalDeviceMemoryProperties	memProps;
	VkSurfaceCapabilitiesKHR			surfaceCaps;
	idList< VkSurfaceFormatKHR >		surfaceFormats;
	idList< VkPresentModeKHR >			presentModes;
	idList< VkQueueFamilyProperties >	queueFamilyProps;
	idList< VkExtensionProperties >		extensionProps;
};

struct vulkanContext_t
{
	uint64							counter;
	uint32							currentFrameData;

	vertCacheHandle_t				jointCacheHandle;

	VkInstance						instance;
	VkPhysicalDevice				physicalDevice;
	VkPhysicalDeviceFeatures		physicalDeviceFeatures;
	VkDevice						device;
	VkQueue							graphicsQueue;
	VkQueue							presentQueue;
	int								graphicsFamilyIdx;
	int								presentFamilyIdx;
	VkDebugReportCallbackEXT		callback;

	idList< const char* >			instanceExtensions;
	idList< const char* >			deviceExtensions;
	idList< const char* >			validationLayers;

	gpuInfo_t* 						gpu;
	idList< gpuInfo_t >				gpus;

	VkCommandPool					commandPool;
	idArray< VkCommandBuffer, NUM_FRAME_DATA >	commandBuffer;
	idArray< VkFence, NUM_FRAME_DATA >			commandBufferFences;
	idArray< bool, NUM_FRAME_DATA >				commandBufferRecorded;

	VkSurfaceKHR					surface;
	VkPresentModeKHR				presentMode;
	VkFormat						depthFormat;
	VkRenderPass					renderPass;
	VkPipelineCache					pipelineCache;
	VkSampleCountFlagBits			sampleCount;
	bool							supersampling;

	int								fullscreen;
	VkSwapchainKHR					swapchain;
	VkFormat						swapchainFormat;
	VkExtent2D						swapchainExtent;
	uint32							currentSwapIndex;
	VkImage							msaaImage;
	VkImageView						msaaImageView;
#if defined( USE_AMD_ALLOCATOR )
	VmaAllocation					msaaVmaAllocation;
	VmaAllocationInfo				msaaAllocation;
#else
	vulkanAllocation_t				msaaAllocation;
#endif
	idArray< VkImage, NUM_FRAME_DATA >			swapchainImages;
	idArray< VkImageView, NUM_FRAME_DATA >		swapchainViews;
	idArray< VkFramebuffer, NUM_FRAME_DATA >	frameBuffers;
	idArray< VkSemaphore, NUM_FRAME_DATA >		acquireSemaphores;
	idArray< VkSemaphore, NUM_FRAME_DATA >		renderCompleteSemaphores;

	int											currentImageParm;
	idArray< idImage*, MAX_IMAGE_PARMS >		imageParms;
};

extern vulkanContext_t vkcontext;

#elif defined( ID_OPENGL )

struct glContext_t
{
	bool		bAnisotropicFilterAvailable;
	bool		bTextureLODBiasAvailable;

	float		maxTextureAnisotropy;

	tmu_t		tmu[ MAX_MULTITEXTURE_UNITS ];
};

extern glContext_t glcontext;

#endif

/*
===========================================================================

idRenderBackend

all state modified by the back end is separated from the front end state

===========================================================================
*/
class idRenderBackend
{
public:
	idRenderBackend();
	~idRenderBackend();

	void				Init();
	void				Shutdown();

	void				ExecuteBackEndCommands( const renderCommand_t* cmds );
	void				BlockingSwapBuffers();

	void				Print();

private:
	void				DrawElementsWithCounters( const drawSurf_t* surf );
	void				DrawStencilShadowPass( const drawSurf_t* drawSurf, const bool renderZPass );

	void				SetColorMappings();
	void				CheckCVars();
	void				ResizeImages();

	void				DrawView( const void* data );
	void				CopyRender( const void* data );

	void				BindVariableStageImage( const textureStage_t* texture, const float* shaderRegisters );
	void				PrepareStageTexturing( const shaderStage_t* pStage, const drawSurf_t* surf );
	void				FinishStageTexturing( const shaderStage_t* pStage, const drawSurf_t* surf );

	void				FillDepthBufferGeneric( const drawSurf_t* const* drawSurfs, int numDrawSurfs );
	void				FillDepthBufferFast( drawSurf_t** drawSurfs, int numDrawSurfs );

	void				T_BlendLight( const drawSurf_t* drawSurfs, const viewLight_t* vLight );
	void				BlendLight( const drawSurf_t* drawSurfs, const drawSurf_t* drawSurfs2, const viewLight_t* vLight );
	void				T_BasicFog( const drawSurf_t* drawSurfs, const idPlane fogPlanes[ 4 ], const idRenderMatrix* inverseBaseLightProject );
	void				FogPass( const drawSurf_t* drawSurfs,  const drawSurf_t* drawSurfs2, const viewLight_t* vLight );
	void				FogAllLights();

	void				DrawInteractions();
	void				DrawSingleInteraction( drawInteraction_t* din );
	int					DrawShaderPasses( const drawSurf_t* const* const drawSurfs, const int numDrawSurfs );

	void				RenderInteractions( const drawSurf_t* surfList, const viewLight_t* vLight, int depthFunc, bool performStencilTest, bool useLightDepthBounds );

	void				StencilShadowPass( const drawSurf_t* drawSurfs, const viewLight_t* vLight );
	void				StencilSelectLight( const viewLight_t* vLight );

private:
	void				GL_StartFrame();
	void				GL_EndFrame();

	uint64				GL_GetCurrentStateMinusStencil() const;
	void				GL_SetDefaultState();
	void				GL_State( uint64 stateBits, bool forceGlState = false );

	void				GL_SelectTexture( int unit );
	void				GL_BindTexture( idImage* image );

	void				GL_CopyFrameBuffer( idImage* image, int x, int y, int imageWidth, int imageHeight );
	void				GL_CopyDepthBuffer( idImage* image, int x, int y, int imageWidth, int imageHeight );
	void				GL_Clear( bool color, bool depth, bool stencil, byte stencilValue, float r, float g, float b, float a );

	void				GL_DepthBoundsTest( const float zmin, const float zmax );
	void				GL_PolygonOffset( float scale, float bias );

	void				GL_Scissor( int x /* left*/, int y /* bottom */, int w, int h );
	void				GL_Viewport( int x /* left */, int y /* bottom */, int w, int h );
	ID_INLINE void		GL_Scissor( const idScreenRect& rect )
	{
		GL_Scissor( rect.x1, rect.y1, rect.x2 - rect.x1 + 1, rect.y2 - rect.y1 + 1 );
	}
	ID_INLINE void		GL_Viewport( const idScreenRect& rect )
	{
		GL_Viewport( rect.x1, rect.y1, rect.x2 - rect.x1 + 1, rect.y2 - rect.y1 + 1 );
	}

	void				GL_Color( float r, float g, float b, float a );
	ID_INLINE void		GL_Color( float r, float g, float b )
	{
		GL_Color( r, g, b, 1.0f );
	}
	void				GL_Color( float* color );

private:
	void				DBG_SimpleSurfaceSetup( const drawSurf_t* drawSurf );
	void				DBG_SimpleWorldSetup();
	void				DBG_ShowDestinationAlpha();
	void				DBG_ColorByStencilBuffer();
	void				DBG_ShowOverdraw();
	void				DBG_ShowIntensity();
	void				DBG_ShowDepthBuffer();
	void				DBG_ShowLightCount();
	void				DBG_EnterWeaponDepthHack();
	void				DBG_EnterModelDepthHack( float depth );
	void				DBG_LeaveDepthHack();
	void				DBG_RenderDrawSurfListWithFunction( drawSurf_t** drawSurfs, int numDrawSurfs );
	void				DBG_ShowSilhouette();
	void				DBG_ShowTris( drawSurf_t** drawSurfs, int numDrawSurfs );
	void				DBG_ShowSurfaceInfo( drawSurf_t** drawSurfs, int numDrawSurfs );
	void				DBG_ShowViewEntitys( viewEntity_t* vModels );
	void				DBG_ShowTexturePolarity( drawSurf_t** drawSurfs, int numDrawSurfs );
	void				DBG_ShowUnsmoothedTangents( drawSurf_t** drawSurfs, int numDrawSurfs );
	void				DBG_ShowTangentSpace( drawSurf_t** drawSurfs, int numDrawSurfs );
	void				DBG_ShowVertexColor( drawSurf_t** drawSurfs, int numDrawSurfs );
	void				DBG_ShowNormals( drawSurf_t** drawSurfs, int numDrawSurfs );
	void				DBG_ShowTextureVectors( drawSurf_t** drawSurfs, int numDrawSurfs );
	void				DBG_ShowDominantTris( drawSurf_t** drawSurfs, int numDrawSurfs );
	void				DBG_ShowEdges( drawSurf_t** drawSurfs, int numDrawSurfs );
	void				DBG_ShowLights();
	void				DBG_ShowPortals();
	void				DBG_ShowDebugText();
	void				DBG_ShowDebugLines();
	void				DBG_ShowDebugPolygons();
	void				DBG_ShowCenterOfProjection();
	void				DBG_TestGamma();
	void				DBG_TestGammaBias();
	void				DBG_TestImage();
	void				DBG_ShowTrace( drawSurf_t** drawSurfs, int numDrawSurfs );
	void				DBG_RenderDebugTools( drawSurf_t** drawSurfs, int numDrawSurfs );

public:
	backEndCounters_t	pc;

	// surfaces used for code-based drawing
	drawSurf_t			unitSquareSurface;
	drawSurf_t			zeroOneCubeSurface;
	drawSurf_t			testImageSurface;

private:
	uint64				glStateBits;

	const viewDef_t* 	viewDef;

	const viewEntity_t*	currentSpace;			// for detecting when a matrix must change
	idScreenRect		currentScissor;		// for scissor clipping, local inside renderView viewport

	bool				currentRenderCopied;	// true if any material has already referenced _currentRender

	idRenderMatrix		prevMVP;				// world MVP from previous frame for motion blur

	unsigned short		gammaTable[ 256 ];	// brightness / gamma modify this

private:
#if defined( ID_OPENGL )
	int					currenttmu;

	unsigned int		currentVertexBuffer;
	unsigned int		currentIndexBuffer;

	float				polyOfsScale;
	float				polyOfsBias;

	idStr				rendererString;
	idStr				vendorString;
	idStr				versionString;
	idStr				extensionsString;
	idStr				wglExtensionsString;
	idStr				shadingLanguageString;

	float				glVersion;			// atof( version_string )
	graphicsVendor_t	vendor;

	int					maxTextureSize;		// queried from GL
	int					maxTextureCoords;
	int					maxTextureImageUnits;
	int					uniformBufferOffsetAlignment;

	int					colorBits;
	int					depthBits;
	int					stencilBits;

	bool				depthBoundsTestAvailable;
	bool				timerQueryAvailable;
	bool				swapControlTearAvailable;

	int					displayFrequency;
#endif
};

#endif