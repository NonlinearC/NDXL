/**
 * @file ndxlcore.h
 *
 * @brief Public API for NDXL error codes and functions, and object definitions.
 *
 * Declares API-wide macros, functions, typedefs, and header files.
 *
 * @copyright Copyright (c) 2026 Ethan Codron
 * @license GNU GPL-3.0-only
*/

#ifndef _NDXLCORE_
#define _NDXLCORE_

#ifdef __cplusplus
#include <fstream>
#include <functional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#endif // __cplusplus

#include "unicpp/unicpp.h"

/**
 * @defgroup ndxl_objects NDXL Objects
 * @brief Opaque type declarations of the objects defined by NDXL.
 *
 * @warning
 * NDXL objects must NOT be freed using free(). Instead, the relevant destructor must be called so that internal links and memory allocations are safely and properly handled. All non-destroyed objects are automatically garbage collected when the program terminates.
 * 
 * @see ndxlInit
 * @see ndxlDestroyNetwork
 * @see ndxlDestroyChannel
 * @see ndxlDestroyVertex
 * @see ndxlDestroyEdge
 * 
 * @{
*/

/**
 * @brief Network object representing a complete simulation system.
 *
 * The central object in NDXL. Each network is its own state machine, managing its constituent vertices, edges, and noise channels, and serving as the target of network simulation functions. Multiple networks can exist simultaneously, but only one can be simulated at a time.
 * 
 * @warning
 * Destroying a network also destroys all channels, vertices, and edges residing within that network, invalidating any pointers to those objects.
 * 
 * @see ndxlCreateNetwork
 * @see ndxlDestroyNetwork
*/
typedef struct NDXLNetwork NDXLNetwork;

/**
 * @brief Noise channel object representing a Wiener process.
 *
 * Each channel defines an independent Wiener process. Noise channels can be shared between multiple agents, and are associated with agents through their noise functions.
 * 
 * @warning
 * Destroying a channel does not affect any other objects in the network, but may disrupt Wiener path preservation and introduce statistical bias. See ndxlCreateChannel() and ndxlDestroyChannel() for further details.
 * 
 * @see ndxlCreateChannel
 * @see ndxlDestroyChannel
 * @see ndxlCreateVertex
 * @see ndxlCreateEdge
*/
typedef struct NDXLChannel NDXLChannel;

/**
 * @brief Vertex object representing an individual resident component of a network.
 *
 * Vertices contain their own state vectors, local dynamics, and noise, and form the nodes of the network topology. Coupling between vertices is made explicit by edges.
 * 
 * @warning
 * Destroying a vertex also destroys all edges coupled to that vertex, invalidating any pointers to those edges. Any channels used by the vertex remain unaffected.
 * 
 * @see ndxlCreateVertex
 * @see ndxlCreateEdge
*/
typedef struct NDXLVertex NDXLVertex;

/**
 * @brief Edge object representing a connection between two vertices.
 *
 * Edges define the relationships and coupling between vertices in a network. Similar to vertices, edges contain their own state functions, local dynamics, and noise. They additionally facilitate delayed and directional information transfer between vertices, and are responsible for historical state storage of the vertices they join.
 * 
 * @warning
 * Destroying an edge also destroys all historical states for the vertices it joins. Any channels used by the edge remain unaffected.
 * 
 * @see ndxlCreateEdge
*/
typedef struct NDXLEdge NDXLEdge;

/** @}*/



#define _NDXL_BEGIN	namespace ndxl {
#define _NDXL_END	}
#define _NDXL		::ndxl::

#ifdef NDXL_STATIC
	#define _NDXLIMP_
#else
	#ifdef _MSC_VER
		#define _NDXLIMP_ __declspec(dllimport)
	#else
		#define _NDXLIMP_ __attribute__((dllimport))
	#endif // _MSC_VER
#endif // NDXL_STATIC



/**
 * @defgroup ndxl_err NDXL Error Codes and Functions
 * 
 * @brief Internal error codes used by NDXL, what they represent, when they are raised, and how to query and respond to them.
 *
 * @details
 * NDXL distinguishes between function return values and error codes.
 *
 * Function return values indicate whether an operation succeeded:
 * - For functions returning numeric values:
 *   - NDXL_OK indicates success.
 *   - NDXL_ERR indicates failure.
 * - For functions returning pointers:
 *   - Non-NULL indicates success.
 *   - NULL (0) indicates failure.
 *
 * The specific cause of an error is recorded separately and can be retrieved using ndxlGetLastErrorCode(), while its associated human-readable description can be retrieved using ndxlGetLastErrorMsg().
 *
 * The error codes NDXL_VALUE_ERR, NDXL_ARG_ERR, and NDXL_SIM_ERR are exclusively used to identify the cause of an error and are never returned directly by functions.
 * 
 * A formatted string combining both pieces of information can be directly printed using ndxlPrintError().
 * 
 * @warning
 * Error information is stored globally and shared between all threads and objects. Consequently, an error raised by one thread or object may overwrite error information previously recorded by another thread or object. Periodic use of error querying while developing a simulation is strongly recommended.
 *
 * @{
*/

/**
 * @brief Successful operation.
 *
 * The "success" indicator for any function that returns an integer.
*/
#define NDXL_OK				0

/**
 * @brief An internal error was raised.
 *
 * The "failure" indicator for any function returning a numeric value.
 * 
 * @note
 * When used as the return value of a function returning size_t, NDXL_ERR is converted to the maximum representable size_t value.
 * 
 * @see ndxlGetLastErrorCode
 * @see ndxlGetErrorMsg
 * @see ndxlPrintError
*/
#define NDXL_ERR			-1

/**
 * @brief Invalid value.
 *
 * Raised whenever an argument had an invalid value, for example:
 * - Negative state vector size in agent creation
 * - Zero step size in simulation function
 * - Mismatched time stops array and count values
 * 
 * @note
 * This value is retrieved through ndxlGetLastErrorCode() and is not returned directly by functions.
 * 
 * @see ndxlCreateVertex
 * @see ndxlCreateEdge
 * @see @ref network_simulation "Network Simulation"
*/
#define NDXL_VALUE_ERR		1

/**
 * @brief Invalid argument.
 *
 * Raised whenever an argument itself was invalid, for example:
 * - Querying the state of a NULL agent
 * - Simulating a NULL network
 * - Passing NULL as the output for list getters
 * 
 * @note
 * This value is retrieved through ndxlGetLastErrorCode() and is not returned directly by functions.
 * 
 * @see ndxlVertexState
 * @see ndxlEdgeState
 * @see @ref network_simulation "Network Simulation"
 * @see ndxlGetNetworkVertices
 * @see ndxlGetNetworkEdges
*/
#define NDXL_ARG_ERR		2

/**
 * @brief Simulation failure.
 *
 * Raised whenever a simulation encounters an external error, triggered by a nonzero return value from a dynamics function, noise function, or callback.
 * 
 * @note
 * This value is retrieved through ndxlGetLastErrorCode() and is not returned directly by functions.
 * 
 * @see ndxlNetworkCallback
 * @see ndxlVertexFunction
 * @see ndxlVertexNoise
 * @see ndxlVertexCallback
 * @see ndxlEdgeFunction
 * @see ndxlEdgeNoise
 * @see ndxlEdgeCallback
*/
#define NDXL_SIM_ERR		3

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Gets the most recently raised error code.
 *
 * @return
 * The most recently raised error code, or NDXL_OK if no error has been raised.
 *
 * @note
 * This function is most commonly used in conjunction with ndxlGetLastErrorMsg for a full error description.
 *
 * @see ndxlGetLastErrorMsg
*/
_NDXLIMP_ const int _CDECL ndxlGetLastErrorCode() _NOEXCEPT;

/**
 * @brief Gets the most recently raised error message.
 *
 * @return
 * Returns the most recently raised error message. If no error has been raised, returns the default initialisation message.
 *
 * @note
 * This function is most commonly used in conjunction with ndxlGetLastErrorCode for a full error description. All error messages end in a fullstop.
 * 
 * @warning
 * The char pointer returned is shared, meaning it will be invalidated if another error is raised. The user must explicitly copy the message contents if they want to preserve the message, however, this is not the intended use for the function.
 * 
 * @see ndxlGetLastErrorCode
*/
_NDXLIMP_ const char* const _CDECL ndxlGetLastErrorMsg() _NOEXCEPT;

/**
 * @brief Prints the most recent error code and message in an easily readable format.
 *
 * @return
 * Returns nothing.
 * 
 * @note
 * NDXL first attempts to write the error to stderr, and falls back to stdout if that fails. If writing to stdout also fails, the function does nothing.
*/
_NDXLIMP_ void _CDECL ndxlPrintError() _NOEXCEPT;

/**
 * @brief Manually sets the library's most recent error.
 *
 * Allows users to explicitly set an NDXL error code and associated error message.
 *
 * @param _Code
 * Error code to set.
 *
 * @param _Msg
 * Null-terminated error message.
 *
 * @return
 * Returns nothing.
 *
 * @note
 * _Msg is copied to the internal error message buffer.
 * 
 * @warning
 * This function is primarily intended for debugging and testing. It does not reproduce the termination behaviour associated with errors raised internally by NDXL.
*/
_NDXLIMP_ void _CDECL ndxlRaiseError(_In_ const int _Code, _In_z_ const char* const _Msg) _NOEXCEPT;

/** @}*/



/**
 * @defgroup lib_mgmt Library Management
 *
 * @brief Core library functions for using and managing the API.
 *
 * @{
*/

/**
 * @brief Initialises the NDXL library.
 *
 * Initialises the library's internal variables and garbage collection.
 *
 * @return
 * NDXL_OK.
 *
 * @warning
 * This function must be called before using any other NDXL functionality. Failure to initialise the library can lead to memory leaks, segmentation faults, and other memory-related errors.
 *
 * @note
 * Registers cleanup functions to be executed when the program terminates. These functions release NDXL-managed resources that have not been explicitly freed by the user.
 *
 * External memory allocations, such as those made using malloc() or calloc(), remain the responsibility of the user, and must be freed explicitly.
*/
_NDXLIMP_ const int _CDECL ndxlInit() _NOEXCEPT;

/**
 * @brief Gets the number of individual NDXLNetwork objects currently alive.
 *
 * @return
 * Returns the number of NDXLNetwork pointers that have not yet been destroyed.
*/
_NDXLIMP_ const size_t _CDECL ndxlGetNetworkCount() _NOEXCEPT;

#ifdef __cplusplus
}
#endif

/** @}*/



#ifdef __cplusplus
_NDXL_BEGIN

class network;
class channel;
class vertex;
class edge;

inline const bool get_last_error_code() noexcept { return ndxlGetLastErrorCode(); }
inline const char* const get_last_error_msg() noexcept { return ndxlGetLastErrorMsg(); }
inline const size_t get_network_count() noexcept { return ndxlGetNetworkCount(); }
inline void print_error() noexcept { ndxlPrintError(); }
inline void raise_error(const int _Code, const char* const _Msg) noexcept { ndxlRaiseError(_Code, _Msg); }
inline const int init() noexcept { return ndxlInit(); }

_NDXL_END
#endif // __cplusplus

#endif // _NDXLCORE_