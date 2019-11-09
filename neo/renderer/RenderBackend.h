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

enum stencilFace_t
{
	STENCIL_FACE_FRONT,
	STENCIL_FACE_BACK,
	STENCIL_FACE_NUM
};

struct backEndCounters_t
{
	backEndCounters_t() :
		c_surfaces( 0 ),
		c_shaders( 0 ),
		c_drawElements( 0 ),
		c_drawIndexes( 0 ),
		c_shadowElements( 0 ),
		c_shadowIndexes( 0 ),
		c_copyFrameBuffer( 0 ),
		c_overDraw( 0 ),
		totalMicroSec( 0 ),
		shadowMicroSec( 0 ),
		interactionMicroSec( 0 ),
		shaderPassMicroSec( 0 ),
		gpuMicroSec( 0 )
	{
	}
	
	int		c_surfaces;
	int		c_shaders;
	
	int		c_drawElements;
	int		c_drawIndexes;
	
	int		c_shadowElements;
	int		c_shadowIndexes;
	
	int		c_copyFrameBuffer;
	
	float	c_overDraw;
	
	uint64	totalMicroSec;		// total microseconds for backend run
	uint64	shadowMicroSec;
	uint64	depthMicroSec;
	uint64	interactionMicroSec;
	uint64	shaderPassMicroSec;
	uint64	gpuMicroSec;
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

/*
===========================================================================

Debug

===========================================================================
*/

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

/*
===========================================================================

RenderProgs

===========================================================================
*/

#define VERTEX_UNIFORM_ARRAY_NAME			"_va_"
#define FRAGMENT_UNIFORM_ARRAY_NAME			"_fa_"

static const int PC_ATTRIB_INDEX_VERTEX		= 0;
static const int PC_ATTRIB_INDEX_NORMAL		= 2;
static const int PC_ATTRIB_INDEX_COLOR		= 3;
static const int PC_ATTRIB_INDEX_COLOR2		= 4;
static const int PC_ATTRIB_INDEX_ST			= 8;
static const int PC_ATTRIB_INDEX_TANGENT	= 9;

enum vertexMask_t {
	VERTEX_MASK_XYZ			= BIT( PC_ATTRIB_INDEX_VERTEX ),
	VERTEX_MASK_ST			= BIT( PC_ATTRIB_INDEX_ST ),
	VERTEX_MASK_NORMAL		= BIT( PC_ATTRIB_INDEX_NORMAL ),
	VERTEX_MASK_COLOR		= BIT( PC_ATTRIB_INDEX_COLOR ),
	VERTEX_MASK_TANGENT		= BIT( PC_ATTRIB_INDEX_TANGENT ),
	VERTEX_MASK_COLOR2		= BIT( PC_ATTRIB_INDEX_COLOR2 ),
};

enum vertexLayoutType_t {
	LAYOUT_UNKNOWN = -1,
	LAYOUT_DRAW_VERT,
	LAYOUT_DRAW_SHADOW_VERT,
	LAYOUT_DRAW_SHADOW_VERT_SKINNED,
	NUM_VERTEX_LAYOUTS
};

// This enum list corresponds to the global constant register indecies as defined in global.inc for all
// shaders.  We used a shared pool to keeps things simple.  If something changes here then it also
// needs to change in global.inc and vice versa
enum renderParm_t {
	// For backwards compatibility, do not change the order of the first 17 items
	RENDERPARM_SCREENCORRECTIONFACTOR = 0,
	RENDERPARM_WINDOWCOORD,
	RENDERPARM_DIFFUSEMODIFIER,
	RENDERPARM_SPECULARMODIFIER,

	RENDERPARM_LOCALLIGHTORIGIN,
	RENDERPARM_LOCALVIEWORIGIN,

	RENDERPARM_LIGHTPROJECTION_S,
	RENDERPARM_LIGHTPROJECTION_T,
	RENDERPARM_LIGHTPROJECTION_Q,
	RENDERPARM_LIGHTFALLOFF_S,

	RENDERPARM_BUMPMATRIX_S,
	RENDERPARM_BUMPMATRIX_T,

	RENDERPARM_DIFFUSEMATRIX_S,
	RENDERPARM_DIFFUSEMATRIX_T,

	RENDERPARM_SPECULARMATRIX_S,
	RENDERPARM_SPECULARMATRIX_T,

	RENDERPARM_VERTEXCOLOR_MODULATE,
	RENDERPARM_VERTEXCOLOR_ADD,

	// The following are new and can be in any order
	
	RENDERPARM_COLOR,
	RENDERPARM_VIEWORIGIN,
	RENDERPARM_GLOBALEYEPOS,

	RENDERPARM_MVPMATRIX_X,
	RENDERPARM_MVPMATRIX_Y,
	RENDERPARM_MVPMATRIX_Z,
	RENDERPARM_MVPMATRIX_W,

	RENDERPARM_MODELMATRIX_X,
	RENDERPARM_MODELMATRIX_Y,
	RENDERPARM_MODELMATRIX_Z,
	RENDERPARM_MODELMATRIX_W,

	RENDERPARM_PROJMATRIX_X,
	RENDERPARM_PROJMATRIX_Y,
	RENDERPARM_PROJMATRIX_Z,
	RENDERPARM_PROJMATRIX_W,

	RENDERPARM_MODELVIEWMATRIX_X,
	RENDERPARM_MODELVIEWMATRIX_Y,
	RENDERPARM_MODELVIEWMATRIX_Z,
	RENDERPARM_MODELVIEWMATRIX_W,

	RENDERPARM_TEXTUREMATRIX_S,
	RENDERPARM_TEXTUREMATRIX_T,

	RENDERPARM_TEXGEN_0_S,
	RENDERPARM_TEXGEN_0_T,
	RENDERPARM_TEXGEN_0_Q,
	RENDERPARM_TEXGEN_0_ENABLED,

	RENDERPARM_TEXGEN_1_S,
	RENDERPARM_TEXGEN_1_T,
	RENDERPARM_TEXGEN_1_Q,
	RENDERPARM_TEXGEN_1_ENABLED,

	RENDERPARM_WOBBLESKY_X,
	RENDERPARM_WOBBLESKY_Y,
	RENDERPARM_WOBBLESKY_Z,

	RENDERPARM_OVERBRIGHT,
	RENDERPARM_ENABLE_SKINNING,
	RENDERPARM_ALPHA_TEST,

	RENDERPARM_USER0,
	RENDERPARM_USER1,
	RENDERPARM_USER2,
	RENDERPARM_USER3,
	RENDERPARM_USER4,
	RENDERPARM_USER5,
	RENDERPARM_USER6,
	RENDERPARM_USER7,

	RENDERPARM_TOTAL
};

const char * GLSLParmNames[];

enum rpBuiltIn_t {
	BUILTIN_GUI,
	BUILTIN_COLOR,
	BUILTIN_SIMPLESHADE,
	BUILTIN_TEXTURED,
	BUILTIN_TEXTURE_VERTEXCOLOR,
	BUILTIN_TEXTURE_VERTEXCOLOR_SKINNED,
	BUILTIN_TEXTURE_TEXGEN_VERTEXCOLOR,
	BUILTIN_INTERACTION,
	BUILTIN_INTERACTION_SKINNED,
	BUILTIN_INTERACTION_AMBIENT,
	BUILTIN_INTERACTION_AMBIENT_SKINNED,
	BUILTIN_ENVIRONMENT,
	BUILTIN_ENVIRONMENT_SKINNED,
	BUILTIN_BUMPY_ENVIRONMENT,
	BUILTIN_BUMPY_ENVIRONMENT_SKINNED,

	BUILTIN_DEPTH,
	BUILTIN_DEPTH_SKINNED,
	BUILTIN_SHADOW,
	BUILTIN_SHADOW_SKINNED,
	BUILTIN_SHADOW_DEBUG,
	BUILTIN_SHADOW_DEBUG_SKINNED,

	BUILTIN_BLENDLIGHT,
	BUILTIN_FOG,
	BUILTIN_FOG_SKINNED,
	BUILTIN_SKYBOX,
	BUILTIN_WOBBLESKY,
	BUILTIN_BINK,
	BUILTIN_BINK_GUI,

	MAX_BUILTINS
};

enum rpBinding_t {
	BINDING_TYPE_UNIFORM_BUFFER,
	BINDING_TYPE_SAMPLER,
	BINDING_TYPE_MAX
};

struct shader_t {
	shader_t() : module( VK_NULL_HANDLE ) {}

	idStr					name;
	rpStage_t				stage;
	VkShaderModule			module;
	idList< rpBinding_t >	bindings;
	idList< int >			parmIndices;
};

struct renderProg_t {
	renderProg_t() :
					usesJoints( false ),
					optionalSkinning( false ),
					vertexShaderIndex( -1 ),
					fragmentShaderIndex( -1 ),
					vertexLayoutType( LAYOUT_DRAW_VERT ),
					pipelineLayout( VK_NULL_HANDLE ),
					descriptorSetLayout( VK_NULL_HANDLE ) {}

	struct pipelineState_t {
		pipelineState_t() : 
					stateBits( 0 ),
					target( NULL ),
					pipeline( VK_NULL_HANDLE ) {
		}

		uint64		stateBits;
		idImage *	target;
		VkPipeline	pipeline;
	};

	VkPipeline GetPipeline( uint64 stateBits, idImage * target, VkShaderModule vertexShader, VkShaderModule fragmentShader );

	idStr						name;
	bool						usesJoints;
	bool						optionalSkinning;
	int							vertexShaderIndex;
	int							fragmentShaderIndex;
	vertexLayoutType_t			vertexLayoutType;
	VkPipelineLayout			pipelineLayout;
	VkDescriptorSetLayout		descriptorSetLayout;
	idList< rpBinding_t >		bindings;
	idList< pipelineState_t >	pipelines;
};

/*
===========================================================================

Backend Globals

===========================================================================
*/
	VkPhysicalDevice					device;
	VkPhysicalDeviceProperties			props;
	VkPhysicalDeviceMemoryProperties	memProps;
	VkPhysicalDeviceFeatures			features;
	VkSurfaceCapabilitiesKHR			surfaceCaps;
	idList< VkSurfaceFormatKHR >		surfaceFormats;
	idList< VkPresentModeKHR >			presentModes;
	idList< VkQueueFamilyProperties >	queueFamilyProps;
	idList< VkExtensionProperties >		extensionProps;
};

struct vulkanContext_t
{
	vertCacheHandle_t				jointCacheHandle;
	
	GPUInfo_t						gpu;
	
	VkDevice						device;
	int								graphicsFamilyIdx;
	int								presentFamilyIdx;
	VkQueue							graphicsQueue;
	VkQueue							presentQueue;
	
	VkFormat						depthFormat;
	VkPipelineCache					pipelineCache;
	VkSampleCountFlagBits			sampleCount;
	bool							supersampling;
	
	idArray< idImage*, MAX_IMAGE_PARMS > imageParms;
};

extern vulkanContext_t vkcontext;

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
	
	int					FindShader( const char * name, rpStage_t stage );
	int					FindProgram( const char * name, int vIndex, int fIndex );

	void				Execute( const int numCmds, const idArray< renderCommand_t, 16 > & renderCommands );
	void				BlockingSwapBuffers();
	
	void				Restart();
	
private:
	bool				OpenWindow();
	void				CloseWindow();

	void				DrawElementsWithCounters( const drawSurf_t* surf );
	void				DrawStencilShadowPass( const drawSurf_t* drawSurf, const bool renderZPass );
	
	void				SetColorMappings();
	void				CheckCVars();
	
	void				DrawView( const renderCommand_t& cmd );
	void				CopyRender( const renderCommand_t& cmd );
	
	void				BindVariableStageImage( const textureStage_t* texture, const float* shaderRegisters );
	void				PrepareStageTexturing( const shaderStage_t* pStage, const drawSurf_t* surf );
	
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
	
	void				GL_StartRenderPass();
	void				GL_EndRenderPass();

	uint64				GL_GetCurrentStateMinusStencil() const;
	void				GL_SetDefaultState();
	void				GL_State( uint64 stateBits, bool forceGlState = false );
	
	void				GL_BindTexture( int index, idImage* image );
	
	void				GL_CopyFrameBuffer( idImage* image, int x, int y, int imageWidth, int imageHeight );
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
	void				BindProgram( int index );
	void				CommitCurrent( uint64 stateBits, idImage * target );

	const idVec4 &		GetRenderParm( renderParm_t rp );
	void				SetRenderParm( renderParm_t rp, const float * value );
	void				SetRenderParms( renderParm_t rp, const float * values, int numValues );

	void				SetVertexColorParms( stageVertexColor_t svc );
	void				LoadShaderTextureMatrix( const float *shaderRegisters, const textureStage_t *texture );

	void				LoadShader( int index );
	void				LoadShader( shader_t & shader );

	void				AllocParmBlockBuffer( const idList< int > & parmIndices, idUniformBuffer & ubo );

private:
	void				Clear();
	
	void				CreateInstance();
	
	void				SelectSuitablePhysicalDevice();
	
	void				CreateLogicalDeviceAndQueues();
	
	void				CreateSemaphores();
	
	void				CreateQueryPool();
	
	void				CreateSurface();
	
	void				CreateCommandPool();
	void				CreateCommandBuffer();
	
	void				CreateSwapChain();
	void				DestroySwapChain();
	
	void				CreateRenderProgs();
	void				DestroyRenderProgs();
	
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
	idImage *			m_currentRenderTarget;
	
	const viewEntity_t*	currentSpace;			// for detecting when a matrix must change
	idScreenRect		currentScissor;		// for scissor clipping, local inside renderView viewport
	
	bool				currentRenderCopied;	// true if any material has already referenced _currentRender
	
	idRenderMatrix		prevMVP;				// world MVP from previous frame for motion blur
	
	unsigned short		gammaTable[ 256 ];	// brightness / gamma modify this
	
private:
	int					m_currentRp;
	int					m_currentDescSet;
	int					m_currentParmBufferOffset;

	idList< shader_t, TAG_RENDER >		m_shaders;
	idList< renderProg_t, TAG_RENDER >	m_renderProgs;
	idStaticList< idVec4, RENDERPARM_TOTAL > m_uniforms;

	VkDescriptorPool	m_descriptorPools[ NUM_FRAME_DATA ];
	VkDescriptorSet		m_descriptorSets[ NUM_FRAME_DATA ][ MAX_DESC_SETS ];

	idUniformBuffer *	m_parmBuffers[ NUM_FRAME_DATA ];

private:
	
	idList< const char* >			instanceExtensions;
	idList< const char* >			deviceExtensions;
	idList< const char* >			validationLayers;
	
	VkSurfaceKHR					surface;
	VkPresentModeKHR				presentMode;
	
	int								fullscreen;
	VkSwapchainKHR					swapchain;
	VkFormat						swapchainFormat;
	VkExtent2D						swapchainExtent;
	uint32							currentSwapIndex;

	idImage *						m_depthAttachment;
	idImage *						m_resolveAttachment;

	idList< idImage * >							m_swapchainImages;
	
	idArray< VkImage, NUM_FRAME_DATA >			swapchainImages;
	idArray< VkImageView, NUM_FRAME_DATA >		swapchainViews;
	idArray< VkFramebuffer, NUM_FRAME_DATA >	frameBuffers;
	
	idArray< VkCommandBuffer, NUM_FRAME_DATA >	commandBuffers;
	idArray< VkFence, NUM_FRAME_DATA >			commandBufferFences;
	idArray< bool, NUM_FRAME_DATA >				commandBufferRecorded;
	idArray< VkSemaphore, NUM_FRAME_DATA >		acquireSemaphores;
	idArray< VkSemaphore, NUM_FRAME_DATA >		renderCompleteSemaphores;
	
	idArray< uint32, NUM_FRAME_DATA >			queryIndex;
	idArray< idArray< uint64, NUM_TIMESTAMP_QUERIES >, NUM_FRAME_DATA >	queryResults;
	idArray< VkQueryPool, NUM_FRAME_DATA >		queryPools;
};

#endif