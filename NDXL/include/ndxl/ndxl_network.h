/**
 * @file ndxl_network.h
 *
 * @brief Public API for NDXL network management and simulation.
 *
 * Declares the NDXLNetwork object and provides functions for network construction, destruction, traversal, parameter management, and numerical simulation.
 *
 * @copyright Copyright (c) 2026 Ethan Codron
 * @license GNU GPL-3.0-only
*/

#ifndef _NDXL_NETWORK_
#define _NDXL_NETWORK_

#include "ndxl_edge.h"

/**
 * @defgroup network_callbacks Network Callbacks
 * @brief Callback signatures for networks during simulation.
 *
 * @{
*/

/**
 * @brief Network callback function.
 *
 * Defines a user-supplied discrete callback that is invoked during network simulation.
 *
 * @param _Network
 * Pointer to the network being simulated.
 *
 * @param _Time
 * Current local simulation time of the network.
 *
 * @param _Params
 * User-defined parameters associated with the network.
 *
 * @return
 * Integer indicating callback success (zero) or failure (any nonzero value).
*/
typedef int(*NDXLNetworkCallback)(NDXLNetwork*, double, void*);

/** @}*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup network_creation Network Constructor and Destructor
 * @brief Functions for creating and destroying networks.
 *
 * @{
*/

/**
 * @brief Creates an empty NDXLNetwork object and returns a pointer to it.
 *
 * @param _Params
 * Pointer to extra memory used by the network. May be NULL.
 *
 * @return
 * If successful, returns a new NDXLNetwork pointer, otherwise returns NULL.
 *
 * @note
 * The local simulation time of a newly created network is always initialised 0, and only updates on a successful integration step.
 * 
 * @warning
 * NDXL stores the supplied `void* _Params` pointer without copying the associated data or managing its lifetime. The pointed-to memory must remain valid for as long as the network uses it. The user remains responsible for managing any allocations.
 *
 * @see ndxlDestroyNetwork
 * @see ndxlGetNetworkTime
 * @see ndxlNetworkSimulateRalston
 * @see ndxlNetworkSimulateNystrom
 * @see ndxlNetworkSimulateDopri
 * @see ndxlNetworkSimulateRkck
 * @see ndxlNetworkSimulateEuler
 * @see ndxlNetworkSimulateHeun
 * @see ndxlNetworkSimulateRadau
*/
_NDXLIMP_ NDXLNetwork* _CDECL ndxlCreateNetwork(
	_In_ _Maybenull_ void* _Params
) _NOEXCEPT;

/**
 * @brief Destroys a network created using ndxlCreateNetwork(), and zeroes the pointer.
 *
 * @param _Network
 * Pointer to NDXLNetwork pointer.
 *
 * @return
 * If _Network or *_Network are NULL, returns NDXL_ERR, otherwise zeroes *_Network, and returns NDXL_OK.
 *
 * @note
 * The network pointer is zeroed after the associated memory is freed, so that subsequent operations using the pointer variable passed to this function can detect that the network has been destroyed.
 * 
 * @warning
 * Destroying a network also destroys all vertices, edges, and noise channels owned by that network. All pointers to the network and its owned objects become invalid. The memory associated with the relevant objects' user-defined `void* _Params` pointers is not handled by the destructors, and must be managed explicitly by the user.
 *
 * Only the pointer variable passed to this function is zeroed. Any other pointers referencing the destroyed network become invalid and must not be used.
 * 
 * @par Errors raised
 * - NDXL_ARG_ERR: _Network is NULL.
 * - NDXL_VALUE_ERR: *_Network is NULL.
 * 
 * @see ndxlCreateNetwork
*/
_NDXLIMP_ const int _CDECL ndxlDestroyNetwork(
	_In_ _Notnull_ NDXLNetwork** _Network
) _NOEXCEPT;

/** @}*/



/**
 * @defgroup network_getters Network Getters
 * @brief Getter functions for querying network properties.
 *
 * @{
*/

/**
 * @brief Gets the number of noise channels a network.
 *
 * @param _Network
 * Target network to retrieve the channel count of.
 *
 * @return
 * If _Network is NULL, returns NDXL_ERR, otherwise returns the number of noise channels in _Network.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Network is NULL.
*/
_NDXLIMP_ const size_t _CDECL ndxlGetNetworkChannelCount(
	_In_ _Notnull_ NDXLNetwork* _Network
) _NOEXCEPT;

/**
 * @brief Gets a list of all the noise channels in a network.
 *
 * @param _Network
 * Target network to retrieve the noise channels of.
 *
 * @param _Channels
 * Pointer to a user-allocated array of NDXLChannel pointers. The array must contain at least ndxlGetNetworkChannelCount(_Network) elements. The function populates the array in-place.
 *
 * @return
 * If either _Network or _Channels are NULL, returns NDXL_ERR, otherwise populates _Channels with the noise channel pointers associated with _Network, and returns NDXL_OK.
 *
 * @note
 * The caller retains ownership of the array, but not of the NDXLChannel objects referenced by its elements. These pointers refer to the original objects in the network, so modifications through them affect those objects directly. The returned list can be used to modify or destroy its referenced objects without invalidating the array itself.
 *
 * @warning
 * The function does not verify the capacity of the supplied array. Providing an inadequately sized array can result in out-of-bounds writes and undefined behaviour.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Network is NULL.
 * - NDXL_ARG_ERR: _Channels is NULL.
 *
 * @see ndxlGetNetworkChannelCount
*/
_NDXLIMP_ const int _CDECL ndxlGetNetworkChannels(
	_In_ _Notnull_ NDXLNetwork* _Network,
	_Out_ _Notnull_ NDXLChannel** _Channels
) _NOEXCEPT;

/**
 * @brief Gets a list of all the edges in a network.
 *
 * @param _Network
 * Target network to retrieve the edges of.
 *
 * @param _Edges
 * Pointer to a user-allocated array of NDXLEdge pointers. The array must contain at least ndxlGetNetworkSize(_Network) elements. The function populates the array in-place.
 *
 * @return
 * If either _Network or _Edges are NULL, returns NDXL_ERR, otherwise populates _Edges with the edge pointers associated with _Network, and returns NDXL_OK.
 *
 * @note
 * The caller retains ownership of the array, but not of the NDXLEdge objects referenced by its elements. These pointers refer to the original objects in the network, so modifications through them affect those objects directly. The returned list can be used to modify or destroy its referenced objects without invalidating the array itself.
 *
 * @warning
 * The function does not verify the capacity of the supplied array. Providing an inadequately sized array can result in out-of-bounds writes and undefined behaviour.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Network is NULL.
 * - NDXL_ARG_ERR: _Edges is NULL.
 *
 * @see ndxlGetNetworkSize
*/
_NDXLIMP_ const int _CDECL ndxlGetNetworkEdges(
	_In_ _Notnull_ NDXLNetwork* _Network,
	_Out_ _Notnull_ NDXLEdge** _Edges
) _NOEXCEPT;

/**
 * @brief Gets the number of vertices in a network.
 *
 * @param _Network
 * Target network to retrieve the order of.
 *
 * @return
 * If _Network is NULL, returns NDXL_ERR, otherwise returns the number of vertices in _Network.
 *
 * @note
 * Not to be confused with ndxlGetNetworkSize, which returns the number of edges in a network.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Network is NULL.
 *
 * @see ndxlGetNetworkSize
*/
_NDXLIMP_ const size_t _CDECL ndxlGetNetworkOrder(
	_In_ _Notnull_ NDXLNetwork* _Network
) _NOEXCEPT;

/**
 * @brief Gets the user-defined `void* _Params` of a network.
 *
 * @param _Network
 * Target network to retrive the parameters of.
 *
 * @return
 * If _Network is NULL, returns NULL, otherwise returns the user-defined `void* _Params` of _Network.
 * 
 * @note
 * A return value of NULL does not necessarily indicate an error, as `void* _Params` could be NULL. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Network is NULL.
 *
 * @see ndxlGetLastErrorCode
 * @see ndxlSetNetworkParams
*/
_NDXLIMP_ void* _CDECL ndxlGetNetworkParams(
	_In_ _Notnull_ NDXLNetwork* _Network
) _NOEXCEPT;

/**
 * @brief Gets the number of edges in a network.
 *
 * @param _Network
 * Target network to retrive the size of.
 *
 * @return
 * If _Network is NULL, returns NDXL_ERR, otherwise returns the number of edges in _Network.
 *
 * @note
 * Not to be confused with ndxlGetNetworkOrder, which returns the number of vertices in a network.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Network is NULL.
 *
 * @see ndxlGetNetworkOrder
*/
_NDXLIMP_ const size_t _CDECL ndxlGetNetworkSize(
	_In_ _Notnull_ NDXLNetwork* _Network
) _NOEXCEPT;

/**
 * @brief Gets the local simulation time of a network.
 *
 * @param _Network
 * Target network to retrive the time of.
 *
 * @return
 * If _Network is NULL, returns NDXL_ERR, otherwise returns the local simulation time of _Network.
 *
 * @note
 * The local simulation time only updates on a successful simulation step.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Network is NULL.
*/
_NDXLIMP_ const double _CDECL ndxlGetNetworkTime(
	_In_ _Notnull_ NDXLNetwork* _Network
) _NOEXCEPT;

/**
 * @brief Gets a list of all the vertices in a network.
 *
 * @param _Network
 * Target network to retrieve the vertices of.
 *
 * @param _Vertices
 * Pointer to a user-allocated array of NDXLVertex pointers. The array must contain at least ndxlGetNetworkOrder(_Network) elements. The function populates the array in-place.
 *
 * @return
 * If either _Network or _Vertices are NULL, returns NDXL_ERR, otherwise populates _Vertices with the vertex pointers associated with _Network, and returns NDXL_OK.
 *
 * @note
 * The caller retains ownership of the array, but not of the NDXLVertex objects referenced by its elements. These pointers refer to the original objects in the network, so modifications through them affect those objects directly. The returned list can be used to modify or destroy its referenced objects without invalidating the array itself.
 *
 * @warning
 * The function does not verify the capacity of the supplied array. Providing an inadequately sized array can result in out-of-bounds writes and undefined behaviour.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Network is NULL.
 * - NDXL_ARG_ERR: _Vertices is NULL.
 *
 * @see ndxlGetNetworkOrder
*/
_NDXLIMP_ const int _CDECL ndxlGetNetworkVertices(
	_In_ _Notnull_ NDXLNetwork* _Network,
	_Out_ _Notnull_ NDXLVertex** _Vertices
) _NOEXCEPT;

/** @}*/



/**
 * @defgroup network_setters Network Setters
 * @brief Setter functions for updating network properties.
 *
 * @{
*/

/**
 * @brief Sets the user-defined `void* _Params` of a network.
 *
 * @param _Network
 * Target network to update the parameters of.
 *
 * @param _Params
 * Pointer to extra memory used by the network. May be NULL.
 *
 * @return
 * If _Network is NULL, returns NDXL_ERR, otherwise updates the parameters of _Network, and returns NDXL_OK.
 *
 * @warning
 * NDXL stores the supplied `void* _Params` pointer without copying the associated data or managing its lifetime. The pointed-to memory must remain valid for as long as the network uses it.
 *
 * This function replaces the networks's existing parameter pointer without freeing or otherwise managing the memory it previously referenced. The user remains responsible for managing both the previous and new allocations.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Network is NULL.
 *
 * @see ndxlGetNetworkParams
*/
_NDXLIMP_ const int _CDECL ndxlSetNetworkParams(
	_In_ _Notnull_ NDXLNetwork* _Network,
	_In_ _Maybenull_ void* _Params
) _NOEXCEPT;

/** @}*/



/**
 * @defgroup network_traversal Network Traversal
 * @brief Functions for traversing network agents and noise channels.
 *
 * @{
*/

/**
 * @brief Gets the first noise channel in a network.
 *
 * @param _Network
 * Target network to retrieve the first noise channel of.
 *
 * @return
 * If _Network is either NULL or has no noise channels, returns NULL, otherwise returns the first NDXLChannel pointer created on _Network.
 *
 * @note
 * This function is most commonly used in conjunction with ndxlGetNetworkNextChannel, ndxlGetNetworkPreviousChannel, and/or ndxlGetNetworkLastChannel to iterate over network noise channels.
 * 
 * A return value of NULL does not necessarily indicate an error, as the network could have no channels. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Network is NULL.
 *
 * @see ndxlGetLastErrorCode
 * @see ndxlGetNetworkNextChannel
 * @see ndxlGetNetworkPreviousChannel
 * @see ndxlGetNetworkLastChannel
*/
_NDXLIMP_ NDXLChannel* _CDECL ndxlGetNetworkFirstChannel(
	_In_ _Notnull_ NDXLNetwork* _Network
) _NOEXCEPT;

/**
 * @brief Gets the first edge in a network.
 *
 * @param _Network
 * Target network to retrieve the first edge of.
 *
 * @return
 * If _Network is either NULL or has no edges, returns NULL, otherwise returns the first NDXLEdge pointer created on _Network.
 *
 * @note
 * This function is most commonly used in conjunction with ndxlGetNetworkNextEdge, ndxlGetNetworkPreviousEdge, and/or ndxlGetNetworkLastEdge to iterate over network edges.
 * 
 * A return value of NULL does not necessarily indicate an error, as the network could have no edges. If unsure, use ndxlGetLastErrorCode() to check.
 * 
 * @par Errors raised
 * - NDXL_ARG_ERR: _Network is NULL.
 *
 * @see ndxlGetLastErrorCode
 * @see ndxlGetNetworkNextEdge
 * @see ndxlGetNetworkPreviousEdge
 * @see ndxlGetNetworkLastEdge
*/
_NDXLIMP_ NDXLEdge* _CDECL ndxlGetNetworkFirstEdge(
	_In_ _Notnull_ NDXLNetwork* _Network
) _NOEXCEPT;

/**
 * @brief Gets the first vertex in a network.
 *
 * @param _Network
 * Target network to retrieve the first vertex of.
 *
 * @return
 * If _Network is either NULL or has no vertices, returns NULL, otherwise returns the first NDXLVertex pointer created on _Network.
 *
 * @note
 * This function is most commonly used in conjunction with ndxlGetNetworkNextVertex, ndxlGetNetworkPreviousVertex, and/or ndxlGetNetworkLastVertex to iterate over network vertices.
 * 
 * A return value of NULL does not necessarily indicate an error, as the network could have no vertices. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Network is NULL.
 *
 * @see ndxlGetLastErrorCode
 * @see ndxlGetNetworkNextVertex
 * @see ndxlGetNetworkPreviousVertex
 * @see ndxlGetNetworkLastVertex
*/
_NDXLIMP_ NDXLVertex* _CDECL ndxlGetNetworkFirstVertex(
	_In_ _Notnull_ NDXLNetwork* _Network
) _NOEXCEPT;

/**
 * @brief Gets the last noise channel in a network.
 *
 * @param _Network
 * Target network to retrieve the last noise channel of.
 *
 * @return
 * If _Network is either NULL or has no noise channels, returns NULL, otherwise returns the last NDXLChannel pointer created on _Network.
 *
 * @note
 * This function is most commonly used in conjunction with ndxlGetNetworkFirstChannel, ndxlGetNetworkNextChannel, and/or ndxlGetNetworkPreviousChannel to iterate over network noise channels.
 * 
 * A return value of NULL does not necessarily indicate an error, as the network could have no channels. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Network is NULL.
 *
 * @see ndxlGetLastErrorCode
 * @see ndxlGetNetworkFirstChannel
 * @see ndxlGetNetworkNextChannel
 * @see ndxlGetNetworkPreviousChannel
*/
_NDXLIMP_ NDXLChannel* _CDECL ndxlGetNetworkLastChannel(
	_In_ _Notnull_ NDXLNetwork* _Network
) _NOEXCEPT;

/**
 * @brief Gets the last edge in a network.
 *
 * @param _Network
 * Target network to retrieve the last edge of.
 *
 * @return
 * If _Network is either NULL or has no edges, returns NULL, otherwise returns the last NDXLEdge pointer created on _Network.
 *
 * @note
 * This function is most commonly used in conjunction with ndxlGetNetworkFirstEdge, ndxlGetNetworkNextEdge, and/or ndxlGetNetworkPreviousEdge to iterate over network edges.
 * 
 * A return value of NULL does not necessarily indicate an error, as the network could have no edges. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Network is NULL.
 *
 * @see ndxlGetLastErrorCode
 * @see ndxlGetNetworkFirstEdge
 * @see ndxlGetNetworkNextEdge
 * @see ndxlGetNetworkPreviousEdge
*/
_NDXLIMP_ NDXLEdge* _CDECL ndxlGetNetworkLastEdge(
	_In_ _Notnull_ NDXLNetwork* _Network
) _NOEXCEPT;

/**
 * @brief Gets the last vertex in a network.
 *
 * @param _Network
 * Target network to retrieve the last vertex of.
 *
 * @return
 * If _Network is either NULL or has no vertices, returns NULL, otherwise returns the last NDXLVertex pointer created on _Network.
 *
 * @note
 * This function is most commonly used in conjunction with ndxlGetNetworkFirstVertex, ndxlGetNetworkNextVertex, and/or ndxlGetNetworkPreviousVertex to iterate over network vertices.
 * 
 * A return value of NULL does not necessarily indicate an error, as the network could have no vertices. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Network is NULL.
 *
 * @see ndxlGetLastErrorCode
 * @see ndxlGetNetworkFirstVertex
 * @see ndxlGetNetworkNextVertex
 * @see ndxlGetNetworkPreviousVertex
*/
_NDXLIMP_ NDXLVertex* _CDECL ndxlGetNetworkLastVertex(
	_In_ _Notnull_ NDXLNetwork* _Network
) _NOEXCEPT;

/**
 * @brief Gets the next channel in the network noise channel sequence.
 *
 * @param _Channel
 * Current NDXLChannel pointer in network noise channel sequence.
 *
 * @return
 * If _Channel is either NULL or is the last noise channel in the network's channel sequence, returns NULL, otherwise returns the next NDXLChannel pointer in the host network's sequence.
 *
 * @note
 * This function is most commonly used in conjunction with ndxlGetNetworkFirstChannel, ndxlGetNetworkPreviousChannel, and/or ndxlGetNetworkLastChannel to iterate over network noise channels.
 * 
 * A return value of NULL does not necessarily indicate an error, as the channel could have no successors. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Channel is NULL.
 *
 * @see ndxlGetLastErrorCode
 * @see ndxlGetNetworkFirstChannel
 * @see ndxlGetNetworkPreviousChannel
 * @see ndxlGetNetworkLastChannel
*/
_NDXLIMP_ NDXLChannel* _CDECL ndxlGetNetworkNextChannel(
	_In_ _Notnull_ NDXLChannel* _Channel
) _NOEXCEPT;

/**
 * @brief Gets the next edge in the network edge sequence.
 *
 * @param _Edge
 * Current NDXLEdge pointer in network edge sequence.
 *
 * @return
 * If _Edge is either NULL or is the last edge in the network's edge sequence, returns NULL, otherwise returns the next NDXLEdge pointer in the host network's sequence.
 *
 * @note
 * This function is most commonly used in conjunction with ndxlGetNetworkFirstEdge, ndxlGetNetworkPreviousEdge, and/or ndxlGetNetworkLastEdge to iterate over network edges.
 * 
 * A return value of NULL does not necessarily indicate an error, as the edge could have no successors. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Edge is NULL.
 *
 * @see ndxlGetLastErrorCode
 * @see ndxlGetNetworkFirstEdge
 * @see ndxlGetNetworkPreviousEdge
 * @see ndxlGetNetworkLastEdge
*/
_NDXLIMP_ NDXLEdge* _CDECL ndxlGetNetworkNextEdge(
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

/**
 * @brief Gets the next vertex in the network vertex sequence.
 *
 * @param _Vertex
 * Current NDXLVertex pointer in network vertex sequence.
 *
 * @return
 * If _Vertex is either NULL or is the last vertex in the network's vertex sequence, returns NULL, otherwise returns the next NDXLVertex pointer in the host network's sequence.
 *
 * @note
 * This function is most commonly used in conjunction with ndxlGetNetworkFirstVertex, ndxlGetNetworkPreviousVertex, and/or ndxlGetNetworkLastVertex to iterate over network vertices.
 * 
 * A return value of NULL does not necessarily indicate an error, as the vertex could have no successors. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Vertex is NULL.
 *
 * @see ndxlGetLastErrorCode
 * @see ndxlGetNetworkFirstVertex
 * @see ndxlGetNetworkPreviousVertex
 * @see ndxlGetNetworkLastVertex
*/
_NDXLIMP_ NDXLVertex* _CDECL ndxlGetNetworkNextVertex(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/**
 * @brief Gets the previous channel in the network noise channel sequence.
 *
 * @param _Channel
 * Current NDXLChannel pointer in network noise channel sequence.
 *
 * @return
 * If _Channel is either NULL or is the first noise channel in the network's channel sequence, returns NULL, otherwise returns the previous NDXLChannel pointer in the host network's sequence.
 *
 * @note
 * This function is most commonly used in conjunction with ndxlGetNetworkFirstChannel, ndxlGetNetworkNextChannel, and/or ndxlGetNetworkLastChannel to iterate over network noise channels.
 * 
 * A return value of NULL does not necessarily indicate an error, as the channel could have no predecessors. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Channel is NULL.
 *
 * @see ndxlGetLastErrorCode
 * @see ndxlGetNetworkFirstChannel
 * @see ndxlGetNetworkNextChannel
 * @see ndxlGetNetworkLastChannel
*/
_NDXLIMP_ NDXLChannel* _CDECL ndxlGetNetworkPreviousChannel(
	_In_ _Notnull_ NDXLChannel* _Channel
) _NOEXCEPT;

/**
 * @brief Gets the previous edge in the network edge sequence.
 *
 * @param _Edge
 * Current NDXLEdge pointer in network edge sequence.
 *
 * @return
 * If _Edge is either NULL or is the first edge in the network's edge sequence, returns NULL, otherwise returns the previous NDXLEdge pointer in the host network's sequence.
 *
 * @note
 * This function is most commonly used in conjunction with ndxlGetNetworkFirstEdge, ndxlGetNetworkNextEdge, and/or ndxlGetNetworkLastEdge to iterate over network edges.
 * 
 * A return value of NULL does not necessarily indicate an error, as the edge could have no predecessors. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Edge is NULL.
 *
 * @see ndxlGetLastErrorCode
 * @see ndxlGetNetworkFirstEdge
 * @see ndxlGetNetworkNextEdge
 * @see ndxlGetNetworkLastEdge
*/
_NDXLIMP_ NDXLEdge* _CDECL ndxlGetNetworkPreviousEdge(
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

/**
 * @brief Gets the previous vertex in the network vertex sequence.
 *
 * @param _Vertex
 * Current NDXLVertex pointer in network vertex sequence.
 *
 * @return
 * If _Vertex is either NULL or is the first vertex in the network's vertex sequence, returns NULL, otherwise returns the previous NDXLVertex pointer in the host network's sequence.
 *
 * @note
 * This function is most commonly used in conjunction with ndxlGetNetworkFirstVertex, ndxlGetNetworkNextVertex, and/or ndxlGetNetworkLastVertex to iterate over network vertices.
 * 
 * A return value of NULL does not necessarily indicate an error, as the vertex could have no predecessors. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Vertex is NULL.
 *
 * @see ndxlGetLastErrorCode
 * @see ndxlGetNetworkFirstVertex
 * @see ndxlGetNetworkNextVertex
 * @see ndxlGetNetworkLastVertex
*/
_NDXLIMP_ NDXLVertex* _CDECL ndxlGetNetworkPreviousVertex(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/** @}*/



/**
 * @defgroup network_simulation Network Simulation
 * @brief Functions for simulating networks.
 * 
 * @par Simulation Behaviour
 * Simulation starts at the local simulation time of a network i.e., the first simulation starts at t = 0, and subsequent simulations on the same network start at whatever time the previous simulation finished.
 * 
 * Each network essentially functions as its own state machine, and because networks can dynamically change during simulation, only the current simulation state is preserved. Any errors encountered during simulation thus leave the network in the state it was in before the error occured.
 * 
 * The evaluation order for a single Runge-Kutta stage is all vertices from ndxlGetNetworkFirstVertex to ndxlGetNetworkLastVertex, followed by all edges from ndxlGetNetworkFirstEdge to ndxlGetNetworkLastEdge.
 *
 * @par Callback Invocation
 * Network callbacks are invoked once at local simulation time t = 0, before integration begins, and subsequently after every successful integration step. The initial callback is only invoked once during the lifetime of a network; repeated calls to simulation functions do not invoke it again.
 *
 * Agent callbacks are invoked after every successful integration step, but are not invoked at the initial simulation time. Like stepping, vertex callbacks are evaluated first, then edge callbacks, in the same order as described above.
 *
 * @par Callback Failure
 * A callback returning any nonzero value indicates failure. NDXL immediately terminates the simulation upon receiving this signal.
 *
 * The network state is not rolled back following a callback failure. Instead, it remains at the state reached when the callback was invoked, with all previously completed integration steps and the network configuration preserved.
 * 
 * @see ndxlGetNetworkFirstVertex
 * @see ndxlGetNetworkLastVertex
 * @see ndxlGetNetworkFirstEdge
 * @see ndxlGetNetworkLastEdge
 * 
 * @{
*/

/**
 * @brief Simulates a network using the static explicit Ralston method of order 4.
 *
 * @param _Network
 * Target network to simulate.
 *
 * @param _Time
 * Duration of the simulation. Must be > 0.
 *
 * @param _Step
 * Step size. Must be > 0.
 *
 * @param _Callback
 * External discrete callback called once at local simulation time of 0, then again after each integration step. May be NULL.
 *
 * @return
 * Returns NDXL_OK if _Network is successfully integrated over _Time, otherwise returns NDXL_ERR.
 *
 * @note
 * This method cannot be used if _Network contains stochastic agents as it is a purely deterministic method.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Network is NULL.
 * - NDXL_VALUE_ERR: _Time <= 0.
 * - NDXL_VALUE_ERR: _Step <= 0.
 * - NDXL_SIM_ERR: _Network contains no agents.
 * - NDXL_SIM_ERR: _Network contains stochastic agents, one of the stochastic methods should be used.
 *
 * @see NDXLNetworkCallback
 * @see ndxlNetworkSimulateEuler
 * @see ndxlNetworkSimulateHeun
*/
_NDXLIMP_ const int _CDECL ndxlNetworkSimulateRalston(
	_In_ _Notnull_ NDXLNetwork* _Network,
	_In_ const double _Time,
	_In_ const double _Step,
	_In_ NDXLNetworkCallback _Callback
) _NOEXCEPT;

/**
 * @brief Simulates a network using the static explicit Nystrom method of order 5.
 *
 * @param _Network
 * Target network to simulate.
 *
 * @param _Time
 * Duration of the simulation. Must be > 0.
 *
 * @param _Step
 * Step size. Must be > 0.
 *
 * @param _Callback
 * External discrete callback called once at local simulation time of 0, then again after each integration step. May be NULL.
 *
 * @return
 * Returns NDXL_OK if _Network is successfully integrated over _Time, otherwise returns NDXL_ERR.
 *
 * @note
 * This method cannot be used if _Network contains stochastic agents as it is a purely deterministic method.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Network is NULL.
 * - NDXL_VALUE_ERR: _Time <= 0.
 * - NDXL_VALUE_ERR: _Step <= 0.
 * - NDXL_SIM_ERR: _Network contains no agents.
 * - NDXL_SIM_ERR: _Network contains stochastic agents, one of the stochastic methods should be used.
 *
 * @see NDXLNetworkCallback
 * @see ndxlNetworkSimulateEuler
 * @see ndxlNetworkSimulateHeun
*/
_NDXLIMP_ const int _CDECL ndxlNetworkSimulateNystrom(
	_In_ _Notnull_ NDXLNetwork* _Network,
	_In_ const double _Time,
	_In_ const double _Step,
	_In_ NDXLNetworkCallback _Callback
) _NOEXCEPT;

/**
 * @brief Simulates a network using the adaptive explicit Dormand-Prince method of order 5(4).
 *
 * @param _Network
 * Target network to simulate.
 *
 * @param _Time
 * Duration of the simulation. Must be > 0.
 *
 * @param _Atol
 * Absolute tolerance (number of correct decimal places). Must be > 0.
 *
 * @param _Rtol
 * Relative tolerance (number of correct digits). Must be > 0.
 *
 * @param _Tstops
 * Array containing times the solver must evaluate. May be 0, in which case _Tcount must also be 0.
 *
 * @param _Tcount
 * Length of _Tstops array. Must be > 0 if _Tstops is provided, and 0 otherwise.
 *
 * @param _Callback
 * External discrete callback called once at local simulation time of 0, then again after each integration step. May be NULL.
 *
 * @return
 * Returns NDXL_OK if _Network is successfully integrated over _Time, otherwise returns NDXL_ERR.
 *
 * @note
 * This method cannot be used if _Network contains stochastic agents as it is a purely deterministic method.
 * 
 * @warning
 * The function does not verify the capacity of the _Tstops array. Providing an inadequately sized array can result in out-of-bounds writes and undefined behaviour.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Network is NULL.
 * - NDXL_VALUE_ERR: _Time <= 0.
 * - NDXL_VALUE_ERR: _Atol <= 0.
 * - NDXL_VALUE_ERR: _Rtol <= 0.
 * - NDXL_VALUE_ERR: _Tstops is NULL and _Tcount is nonzero, or _Tstops is not NULL and _Tcount is 0.
 * - NDXL_SIM_ERR: _Network contains no agents.
 * - NDXL_SIM_ERR: _Network contains stochastic agents, one of the stochastic methods should be used.
 *
 * @see NDXLNetworkCallback
 * @see ndxlNetworkSimulateEuler
 * @see ndxlNetworkSimulateHeun
*/
_NDXLIMP_ const int _CDECL ndxlNetworkSimulateDopri(
	_In_ _Notnull_ NDXLNetwork* _Network,
	_In_ const double _Time,
	_In_ const double _Atol,
	_In_ const double _Rtol,
	_In_reads_bytes_(_Tcount * sizeof(double)) const double* _Tstops,
	_In_ const size_t _Tcount,
	_In_ NDXLNetworkCallback _Callback
) _NOEXCEPT;

/**
 * @brief Simulates a network using the adaptive explicit Cash-Karp method of order 5(4).
 *
 * @param _Network
 * Target network to simulate.
 *
 * @param _Time
 * Duration of the simulation. Must be > 0.
 *
 * @param _Atol
 * Absolute tolerance (number of correct decimal places). Must be > 0.
 *
 * @param _Rtol
 * Relative tolerance (number of correct digits). Must be > 0.
 *
 * @param _Tstops
 * Array containing times the solver must evaluate. May be NULL, in which case _Tcount must also be 0.
 *
 * @param _Tcount
 * Length of _Tstops array. Must be > 0 if _Tstops is provided, and 0 otherwise.
 *
 * @param _Callback
 * External discrete callback called once at local simulation time of 0, then again after each integration step. May be NULL.
 *
 * @return
 * Returns NDXL_OK if _Network is successfully integrated over _Time, otherwise returns NDXL_ERR.
 *
 * @note
 * This method cannot be used if _Network contains stochastic agents as it is a purely deterministic method.
 * 
 * @warning
 * The function does not verify the capacity of the _Tstops array. Providing an inadequately sized array can result in out-of-bounds writes and undefined behaviour.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Network is NULL.
 * - NDXL_VALUE_ERR: _Time <= 0.
 * - NDXL_VALUE_ERR: _Atol <= 0.
 * - NDXL_VALUE_ERR: _Rtol <= 0.
 * - NDXL_VALUE_ERR: _Tstops is NULL and _Tcount is nonzero, or _Tstops is not NULL and _Tcount is 0.
 * - NDXL_SIM_ERR: _Network contains no agents.
 * - NDXL_SIM_ERR: _Network contains stochastic agents, one of the stochastic methods should be used.
 *
 * @see NDXLNetworkCallback
 * @see ndxlNetworkSimulateEuler
 * @see ndxlNetworkSimulateHeun
*/
_NDXLIMP_ const int _CDECL ndxlNetworkSimulateRkck(
	_In_ _Notnull_ NDXLNetwork* _Network,
	_In_ const double _Time,
	_In_ const double _Atol,
	_In_ const double _Rtol,
	_In_reads_bytes_(_Tcount * sizeof(double)) const double* _Tstops,
	_In_ const size_t _Tcount,
	_In_ NDXLNetworkCallback _Callback
) _NOEXCEPT;

/**
 * @brief Simulates a network using the static explicit Euler-Maruyama method of strong order 0.5.
 *
 * @param _Network
 * Target network to simulate.
 *
 * @param _Time
 * Duration of the simulation. Must be > 0.
 *
 * @param _Step
 * Step size. Must be > 0.
 *
 * @param _Callback
 * External discrete callback called once at local simulation time of 0, then again after each integration step. May be NULL.
 *
 * @return
 * Returns NDXL_OK if _Network is successfully integrated over _Time, otherwise returns NDXL_ERR.
 *
 * @note
 * This method cannot be used if _Network contains no stochastic agents as it is a stochastic method.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Network is NULL.
 * - NDXL_VALUE_ERR: _Time <= 0.
 * - NDXL_VALUE_ERR: _Step <= 0.
 * - NDXL_SIM_ERR: _Network contains no agents.
 * - NDXL_SIM_ERR: _Network contains no stochastic agents, one of the deterministic methods should be used.
 *
 * @see NDXLNetworkCallback
 * @see ndxlNetworkSimulateRalston
 * @see ndxlNetworkSimulateNystrom
 * @see ndxlNetworkSimulateDopri
 * @see ndxlNetworkSimulateRkck
 * @see ndxlNetworkSimulateRadau
*/
_NDXLIMP_ const int _CDECL ndxlNetworkSimulateEuler(
	_In_ _Notnull_ NDXLNetwork* _Network,
	_In_ const double _Time,
	_In_ const double _Step,
	_In_ NDXLNetworkCallback _Callback
) _NOEXCEPT;

/**
 * @brief Simulates a network using the adaptive explicit Euler-Heun method of strong order 1(0.5). Uses rejection sampling with memory (RSwM) to preserve Wiener path distributions.
 *
 * @param _Network
 * Target network to simulate.
 *
 * @param _Time
 * Duration of the simulation. Must be > 0.
 *
 * @param _Atol
 * Absolute tolerance (number of correct decimal places). Must be > 0.
 *
 * @param _Rtol
 * Relative tolerance (number of correct digits). Must be > 0.
 *
 * @param _Tstops
 * Array containing times the solver must evaluate. May be NULL, in which case _Tcount must also be 0.
 *
 * @param _Tcount
 * Length of _Tstops array. Must be > 0 if _Tstops is provided, and 0 otherwise.
 *
 * @param _Callback
 * External discrete callback called once at local simulation time of 0, then again after each integration step. May be NULL.
 *
 * @return
 * Returns NDXL_OK if _Network is successfully integrated over _Time, otherwise returns NDXL_ERR.
 *
 * @note
 * This method cannot be used if _Network contains no stochastic agents as it is a stochastic method.
 * 
 * @warning
 * The function does not verify the capacity of the _Tstops array. Providing an inadequately sized array can result in out-of-bounds writes and undefined behaviour.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Network is NULL.
 * - NDXL_VALUE_ERR: _Time <= 0.
 * - NDXL_VALUE_ERR: _Atol <= 0.
 * - NDXL_VALUE_ERR: _Rtol <= 0.
 * - NDXL_VALUE_ERR: _Tstops is NULL and _Tcount is nonzero, or _Tstops is not NULL and _Tcount is 0.
 * - NDXL_SIM_ERR: _Network contains no agents.
 * - NDXL_SIM_ERR: _Network contains no stochastic agents, one of the deterministic methods should be used.
 *
 * @see NDXLNetworkCallback
 * @see ndxlNetworkSimulateRalston
 * @see ndxlNetworkSimulateNystrom
 * @see ndxlNetworkSimulateDopri
 * @see ndxlNetworkSimulateRkck
 * @see ndxlNetworkSimulateRadau
*/
_NDXLIMP_ const int _CDECL ndxlNetworkSimulateHeun(
	_In_ _Notnull_ NDXLNetwork* _Network,
	_In_ const double _Time,
	_In_ const double _Atol,
	_In_ const double _Rtol,
	_In_reads_bytes_(_Tcount * sizeof(double)) const double* _Tstops,
	_In_ const size_t _Tcount,
	_In_ NDXLNetworkCallback _Callback
) _NOEXCEPT;

//_NDXLIMP_ const int _CDECL ndxlNetworkSimulateNorsett(
//	_In_ _Notnull_ NDXLNetwork* _Network,
//	_In_ const double _Time,
//	_In_ const double _Step,
//	_In_ NDXLNetworkCallback _Callback
//) _NOEXCEPT;

/**
 * @brief Simulates a network using the adaptive implicit Radau IIA method of order 5(3).
 *
 * @param _Network
 * Target network to simulate.
 *
 * @param _Time
 * Duration of the simulation. Must be > 0.
 *
 * @param _Atol
 * Absolute tolerance (number of correct decimal places). Must be > 0.
 *
 * @param _Rtol
 * Relative tolerance (number of correct digits). Must be > 0.
 *
 * @param _Tstops
 * Array containing times the solver must evaluate. May be NULL, in which case _Tcount must also be 0.
 *
 * @param _Tcount
 * Length of _Tstops array. Must be > 0 if _Tstops is provided, and 0 otherwise.
 *
 * @param _Callback
 * External discrete callback called once at local simulation time of 0, then again after each integration step. May be NULL.
 *
 * @return
 * Returns NDXL_OK if _Network is successfully integrated over _Time, otherwise returns NDXL_ERR.
 *
 * @note
 * This method cannot be used if _Network contains stochastic agents as it is a purely deterministic method.
 * 
 * @warning
 * The function does not verify the capacity of the _Tstops array. Providing an inadequately sized array can result in out-of-bounds writes and undefined behaviour.
 * 
 * This method relies on network-wide Jacobian evaluations, meaning memory usage scales with the number of agents squared, as opposed to the linear scaling seen in explicit methods. 
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Network is NULL.
 * - NDXL_VALUE_ERR: _Time <= 0.
 * - NDXL_VALUE_ERR: _Atol <= 0.
 * - NDXL_VALUE_ERR: _Rtol <= 0.
 * - NDXL_VALUE_ERR: _Tstops is NULL and _Tcount is nonzero, or _Tstops is not NULL and _Tcount is 0.
 * - NDXL_SIM_ERR: _Network contains no agents.
 * - NDXL_SIM_ERR: _Network contains stochastic agents, one of the stochastic methods should be used.
 *
 * @see NDXLNetworkCallback
 * @see ndxlNetworkSimulateEuler
 * @see ndxlNetworkSimulateHeun
*/
_NDXLIMP_ const int _CDECL ndxlNetworkSimulateRadau(
	_In_ _Notnull_ NDXLNetwork* _Network,
	_In_ const double _Time,
	_In_ const double _Atol,
	_In_ const double _Rtol,
	_In_reads_bytes_(_Tcount * sizeof(double)) const double* _Tstops,
	_In_ const size_t _Tcount,
	_In_ NDXLNetworkCallback _Callback
) _NOEXCEPT;

/** @}*/

#ifdef __cplusplus
}
#endif

//#ifdef __cplusplus
//_NDXL_BEGIN
//
//using network_callback = std::function<void(const network&, double, const std::vector<any>&)>;
//
//int _nwk_bridge(NDXLNetwork* _Network, const double _Time, void* _Params);
//
//class network
//{
//public:
//	inline network(NDXLNetwork* _Ptr) noexcept : _My_ptr(_Ptr) {}
//
//	inline virtual ~network() noexcept
//	{
//		this->_My_ptr = 0;
//	}
//
//	inline const int destroy() noexcept
//	{
//		network::_Nwkargs.erase(this->_My_ptr);
//		return ndxlDestroyNetwork(&(this->_My_ptr));
//	}
//
//protected:
//	NDXLNetwork* _My_ptr;
//
//public:
//	static std::unordered_map<NDXLNetwork*, std::vector<std::any>> _Nwkargs;
//
//public:
//	inline void _Destroy_vertex_params(NDXLVertex* _Vertex) noexcept
//	{
//		network::_Nwkargs[this->_My_ptr][1].erase(_Vertex);
//		ndxlSetVertexParams(_Vertex, 0);
//	}
//
//	inline void _Destroy_edge_params(NDXLEdge* _Edge) noexcept
//	{
//		network::_Nwkargs[this->_My_ptr][2].erase(_Edge);
//		ndxlSetEdgeParams(_Edge, 0);
//	}
//
//	inline NDXLNetwork* c_ptr() const noexcept
//	{
//		return this->_My_ptr;
//	}
//
//	inline const std::vector<edge>& edges() const noexcept
//	{
//		std::vector<edge> _Edges(ndxlGetNetworkSize(this->_My_ptr));
//		ndxlGetNetworkEdges(this->_My_ptr, &(_Edges->data));
//		return _Edges;
//	}
//
//	inline const std::size_t order() const noexcept
//	{
//		return ndxlGetNetworkOrder(this->_My_ptr);
//	}
//
//	inline std::vector<any>& params() noexcept
//	{
//		return network::_Nwkargs[this->_My_ptr][3];
//	}
//
//	inline const std::size_t size() const noexcept
//	{
//		return ndxlGetNetworkSize(this->_My_ptr);
//	}
//
//	inline const double time() const noexcept
//	{
//		return ndxlGetNetworkTime(this->_My_ptr);
//	}
//
//	inline const std::vector<vertex>& vertices() const noexcept
//	{
//		std::vector<vertex> _Vertices(ndxlGetNetworkOrder(this->_My_ptr));
//		ndxlGetNetworkVertices(this->_My_ptr, &(_Vertices->data));
//		return _Vertices;
//	}
//
//	inline const int reset() noexcept
//	{
//		return ndxlNetworkReset(this->_My_ptr);
//	}
//
//	inline const int simulate_ralston(const double _Tspan, const double _Step = 0.1, const network_callback& _Cb = nullptr) noexcept
//	{
//		network::_Nwkargs[this->_My_ptr][0] = _Cb;
//		return ndxlNetworkSimulateRalston(this->_My_ptr, _Tspan, _Step, _nwk_bridge);
//	}
//
//	inline const int simulate_nystrom(const double _Tspan, const double _Step = 0.1, const network_callback& _Cb = nullptr) noexcept
//	{
//		network::_Nwkargs[this->_My_ptr][0] = _Cb;
//		return ndxlNetworkSimulateNystrom(this->_My_ptr, _Tspan, _Step, _nwk_bridge);
//	}
//
//	inline const int simulate_dopri(const double _Tspan, const double _Atol = 1e-6, const double _Rtol = 1e-3, const std::vector<double>& _Tstops = {}, const network_callback& _Cb = nullptr) noexcept
//	{
//		network::_Nwkargs[this->_My_ptr][0] = _Cb;
//		const std::size_t _Nstops = _Tstops.size();
//		const double* _Stops = (_Nstops ? _Tstops.data() : 0);
//		return ndxlNetworkSimulateDopri(this->_My_ptr, _Tspan, _Atol, _Rtol, _Stops, _Nstops, _nwk_bridge);
//	}
//
//	inline const int simulate_rkck(const double _Tspan, const double _Atol = 1e-6, const double _Rtol = 1e-3, const std::vector<double>& _Tstops = {}, const network_callback& _Cb = nullptr) noexcept
//	{
//		network::_Nwkargs[this->_My_ptr][0] = _Cb;
//		const std::size_t _Nstops = _Tstops.size();
//		const double* _Stops = (_Nstops ? _Tstops.data() : 0);
//		return ndxlNetworkSimulateRkck(this->_My_ptr, _Tspan, _Atol, _Rtol, _Stops, _Nstops, _nwk_bridge);
//	}
//
//	inline const int simulate_norsett(const double _Tspan, const double _Step = 0.1, const network_callback& _Cb = nullptr) noexcept
//	{
//		//network::_Nwkargs[this->_My_ptr][0] = _Cb;
//		//return ndxlNetworkSimulateNorsett(this->_My_ptr, _Tspan, _Step, _nwk_bridge);
//		return NDXL_ERR;
//	}
//
//	inline const int simulate_radau(const double _Tspan, const double _Atol = 1e-10, const double _Rtol = 1e-8, const std::vector<double>& _Tstops = {}, const network_callback& _Cb = nullptr) noexcept
//	{
//		//network::_Nwkargs[this->_My_ptr][0] = _Cb;
//		//const std::size_t _Nstops = _Tstops.size();
//		//const double* _Stops = (_Nstops ? _Tstops.data() : 0);
//		//return ndxlNetworkSimulateRadau(this->_My_ptr, _Tspan, _Atol, _Rtol, _Stops, _Nstops, _nwk_bridge);
//		return NDXL_ERR;
//	}
//};
//
//std::unordered_map<NDXLNetwork*, std::vector<any>> network::_Nwkargs = {};
//
//inline network create_network(const std::vector<any>& _Args = {})
//{
//	NDXLNetwork* _Nwk = ndxlCreateNetwork(0);
//	network::_Nwkargs[_Nwk] = { network_callback(nullptr), std::unordered_map<NDXLVertex*, std::vector<std::any>>(), std::unordered_map<NDXLEdge*, std::vector<std::any>>(), _Args };
//	ndxlSetNetworkParams(_Nwk, _Nwk);
//	return network(_Nwk);
//}
//
//int _nwk_bridge(NDXLNetwork* _Network, const double _Time, void* _Params)
//{
//	try
//	{
//		const std::vector<any>& _Args = network::_Nwkargs[(NDXLNetwork*)_Params];
//		const network_callback& _Cb = _Args[0];
//		if (_Cb)
//			_Cb(network(_Network), _Time, _Args[3]);
//		return 0;
//	}
//	catch (std::exception& _Exc)
//	{
//		return -1;
//	}
//}
//
//_NDXL_END
//#endif

#endif // _NDXL_NETWORK_