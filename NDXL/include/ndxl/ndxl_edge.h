/**
 * @file ndxl_edge.h
 *
 * @brief Public API for NDXL edge creation, state and parameter management, and vertex inspection.
 *
 * Declares the NDXLEdge object, representing an edge in a network. Provides functions for edge construction, destruction, vertex inspection, and state and parameter querying.
 *
 * @copyright Copyright (c) 2026 Ethan Codron
 * @license GNU GPL-3.0-only
*/

#ifndef _NDXL_EDGE_
#define _NDXL_EDGE_

#include "ndxl_vertex.h"

/**
 * @defgroup edge_dnc Edge Dynamics, Noise, and Callbacks
 * @brief Dynamics, noise, and callback functions for edge objects.
 *
 * @{
*/

/**
 * @brief Edge dynamics function.
 *
 * Defines a user-supplied function that governs how the state of an edge evolves over time.
 *
 * @param NDXLEdge*
 * The calling edge.
 *
 * @param double*
 * Flat output vector containing the time derivative of the calling edge. ndxlGetEdgeOrder(calling_edge) total elements must be written to it, even if those elements are zero.
 *
 * @param const double*
 * Flat input vector containing the current Runge-Kutta stage of the calling edge. Is guaranteed to have ndxlGetEdgeOrder(calling_edge) elements.
 *
 * @param double
 * Local simulation time of the calling edge, offset by a fraction of the step size corresponding to the next Runge-Kutta stage. Note that this is generally NOT equivalent to ndxlGetEdgeTime(calling_edge), which only updates on a successful step.
 *
 * @param void*
 * User-defined parameters associated with the calling edge.
 *
 * @return
 * Integer indicating function success (zero) or failure (any nonzero value).
 *
 * @warning
 * The function does not verify the number of elements written to the output array. Not writing to each element can result in garbage values or stale states being propagated. Writing beyond ndxlGetEdgeOrder(calling_edge) elements can result in out-of-bounds writes and undefined behaviour.
 *
 * @see ndxlGetEdgeOrder
*/
typedef int(*NDXLEdgeDynamics)(NDXLEdge*, double*, const double*, double, void*);

/**
 * @brief Edge noise function.
 *
 * Defines a user-supplied function that provides the amplitude of noise, for each element in its state vector, for each independent Wiener process the edge is driven by.
 *
 * @param NDXLEdge*
 * The calling edge.
 *
 * @param double*
 * Flat output matrix containing the noise amplitudes, where element ij (at index i * ndxlGetEdgeChannelCount(calling_edge) + j) represents the amplitude of the j-th Wiener process on the i-th state-vector element. ndxlGetEdgeOrder(calling_edge) x ndxlGetEdgeChannelCount(calling_edge) total elements must be written to it, even if those elements are zero.
 *
 * @param const double*
 * Flat input vector containing the current Runge-Kutta stage of the calling edge. Is guaranteed to have ndxlGetEdgeOrder(calling_edge) elements.
 *
 * @param double
 * Local simulation time of the calling edge, offset by a fraction of the step size corresponding to the next Runge-Kutta stage. Note that this is generally NOT equivalent to ndxlGetEdgeTime(calling_edge), which only updates on a successful step.
 *
 * @param void*
 * User-defined parameters associated with the calling edge.
 *
 * @return
 * Integer indicating function success (zero) or failure (any nonzero value).
 *
 * @warning
 * The function does not verify the number of elements written to the output array. Not writing to each element can result in garbage values or stale states being propagated. Writing beyond ndxlGetEdgeOrder(calling_edge) x ndxlGetEdgeChannelCount(calling_edge) elements can result in out-of-bounds writes and undefined behaviour.
 *
 * @see ndxlGetEdgeOrder
 * @see ndxlGetEdgeChannelCount
*/
typedef int(*NDXLEdgeNoise)(NDXLEdge*, double*, const double*, double, void*);

/**
 * @brief Edge callback function.
 *
 * Defines a user-supplied discrete callback that is invoked during simulation of the edge's host network.
 *
 * @param NDXLEdge*
 * The calling edge.
 *
 * @param double*
 * Flat vector containing the most recently accepted state vector of the calling edge. Is guaranteed to have ndxlGetEdgeOrder(calling_edge) elements, and may be modified in-place.
 *
 * @param double
 * Current simulation time of the calling edge, equivalent to ndxlGetEdgeTime(calling_edge).
 *
 * @param void*
 * User-defined parameters associated with the calling edge.
 *
 * @return
 * Integer indicating function success (zero) or failure (any nonzero value).
 *
 * @warning
 * Writing beyond ndxlGetEdgeOrder(calling_edge) elements to the state vector can result in out-of-bounds writes and undefined behaviour.
 *
 * @see ndxlGetEdgeOrder
*/
typedef int(*NDXLEdgeCallback)(NDXLEdge*, double*, double, void*);

/** @}*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup edge_creation Edge Constructor and Destructor
 * @brief Functions for creating and destroying edges.
 *
 * @{
*/

/**
 * @brief Creates an edge between two vertices in the same network and returns a pointer to it.
 *
 * @param _Vert1
 * The `source` vertex.
 * 
 * @param _Vert2
 * The `destination` vertex. Must be part of the same network as _Vert1 i.e., ndxlGetVertexNetwork(_Vert1) == ndxlGetVertexNetwork(_Vert2). May be the same vertex as _Vert1, which creates a self-loop.
 * 
 * @param _State
 * Flat initial state vector at time 0. May be NULL, in which case a zero-vector of size max(1, _Order) is assigned by default.
 * 
 * @param _Order
 * Size of state vector. If _State is provided, must be nonzero.
 * 
 * @param _Delay
 * The time taken for information to travel from _Vert1 to _Vert2. _Must be greater than or equal to 0. _Historical state information for _Vert1 is then stored up to t - _Delay.
 * 
 * If _Directed is falsey, this is also the time taken for information to travel from _Vert2 to _Vert1, and historical state information for _Vert2 is stored up to t - _Delay.
 * 
 * @param _Directed
 * If truthy, information flows only from _Vert1 to _Vert2, otherwise information flow is bidirectional.
 * 
 * @param _Dynamics
 * The dynamics function describing how the state vector evolves over time. May be NULL, in which case the time derivative is zero (static edge).
 * 
 * @param _Noise
 * The noise function describing the noise amplitude over time. May be NULL, in which case _Channels must be NULL and _Nchannels must be 0.
 * 
 * @param _Channels
 * Array of NDXLChannel pointers defining which noise channels affect this edge through _Noise. May be NULL unless _Nchannels is nonzero. If provided, any element may be NULL, which is interpreted as a new channel.
 * - If _Channels is NULL and _Nchannels is 0, creates _Order new noise channels and assigns them to this edge. The noise matrix has shape _Order x _Order.
 * - If _Channels is NULL and _Nchannels is nonzero, creates _Nchannels new noise channels and assigns them to this edge. The noise matrix has shape _Order x _Nchannels.
 * - If any element of _Channels is NULL, creates a new noise channel for that element and assigns it to this edge. The noise matrix has shape _Order x _Nchannels.
 * 
 * @param _Callback
 * External discrete callback called once after each successful integration step. May be NULL.
 * 
 * @param _Params
 * Pointer to extra memory used by the edge. May be NULL.
 *
 * @return
 * If successful, returns a new NDXLEdge pointer, otherwise returns NULL.
 *
 * @note
 * If _Vert1 and _Vert2 point to the same NDXLVertex, the edge forms a self-loop, which can be useful for defining delay differential equations (DDEs) where the state of the vertex depends on its own past state [see ndxlGetEdgeRelativeState()].
 * 
 * Multiple edges can exist simultaneously between _Vert1 and _Vert2, and can have different delays, directedness, and state vectors. This is primarily useful for extending DDEs to depend on past states at different intervals. [see ndxlGetEdgeRelativeState()].
 * 
 * In general, any edges in the network may be directed or undirected. However, the delay and directedness of an edge cannot be altered after creation.
 * 
 * The local simulation time of a newly created edge is always initialised 0, and only updates on a successful integration step.
 * 
 * Unlike network callbacks, edge callbacks are NOT called at time 0.
 *
 * @warning
 * The function does not verify the capacity of the supplied state and channel arrays. Both arrays must contain at least _Order and _Nchannels elements, respectively, whenever they are used. Incorrect array sizes or inconsistent _Order and _Nchannels arguments can result in out-of-bounds reads and undefined behaviour.
 * 
 * NDXL stores the supplied `void* _Params` pointer without copying the associated data or managing its lifetime. The pointed-to memory must remain valid for as long as the edge uses it. The user remains responsible for managing any allocations.
 *
 * @see NDXLEdgeDynamics
 * @see NDXLEdgeNoise
 * @see NDXLEdgeCallback
 * @see ndxlDestroyEdge
 * @see ndxlGetVertexNetwork
 * @see ndxlGetEdgeRelativeState
*/
_NDXLIMP_ NDXLEdge* _CDECL ndxlCreateEdge(
	_In_ _Notnull_ NDXLVertex* _Vert1,
	_In_ _Notnull_ NDXLVertex* _Vert2,
	_In_reads_bytes_opt_(_Order * sizeof(double)) const double* _State,
	_In_ const size_t _Order,
	_In_ const double _Delay,
	_In_ const char _Directed,
	_In_ _Maybenull_ NDXLEdgeDynamics _Dynamics,
	_In_ _Maybenull_ NDXLEdgeNoise _Noise,
	_In_reads_bytes_opt_(_Nchannels * sizeof(NDXLChannel*)) NDXLChannel** _Channels,
	_In_ const size_t _Nchannels,
	_In_ _Maybenull_ NDXLEdgeCallback _Callback,
	_In_ _Maybenull_ void* _Params
) _NOEXCEPT;

/**
 * @brief Destroys an edge created using ndxlCreateEdge(), and zeroes the pointer.
 *
 * @param _Edge
 * Pointer to NDXLEdge pointer.
 *
 * @return
 * If _Edge or *_Edge are NULL, returns NDXL_ERR, otherwise zeroes *_Edge, and returns NDXL_OK.
 *
 * @note
 * The edge pointer is zeroed after the associated memory is freed, so that subsequent operations using the pointer variable passed to this function can detect that the edge has been destroyed.
 *
 * @warning
 * Destroying an edge also destroys the historical state information maintained by that edge for its connected vertices. The memory associated with the edge's user-defined `void* _Params` pointer is not handled by the destructor, and must be managed explicitly by the user.
 * 
 * Only the pointer variable passed to this function is zeroed. Any other pointers referencing the destroyed edge become invalid and must not be used.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Edge is NULL.
 * - NDXL_VALUE_ERR: *_Edge is NULL.
 *
 * @see ndxlCreateEdge
*/
_NDXLIMP_ const int _CDECL ndxlDestroyEdge(
	_In_ _Notnull_ NDXLEdge** _Edge
) _NOEXCEPT;

/** @}*/



/**
 * @defgroup edge_getters Edge Getters
 * @brief Getter functions for querying edge properties.
 *
 * @{
*/

/**
 * @brief Gets the callback function associated with an edge.
 *
 * @param _Edge
 * Target edge to retrieve the callback of.
 *
 * @return
 * If _Edge is NULL, returns NULL, otherwise returns the callback function associated with _Edge.
 *
 * @note
 * A return value of NULL does not necessarily indicate an error, as the callback function could be NULL. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Edge is NULL.
 *
 * @see ndxlGetLastErrorCode
 * @see ndxlSetEdgeCallback
*/
_NDXLIMP_ NDXLEdgeCallback _CDECL ndxlGetEdgeCallback(
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

/**
 * @brief Gets the number of noise channels affecting an edge.
 *
 * @param _Edge
 * Target edge to retrieve the channel count of.
 *
 * @return
 * If _Edge is NULL, returns NDXL_ERR, otherwise returns the number of noise channels affecting _Edge.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Edge is NULL.
*/
_NDXLIMP_ const size_t _CDECL ndxlGetEdgeChannelCount(
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

/**
 * @brief Gets a list of all the noise channels affecting an edge.
 *
 * @param _Edge
 * Target edge to retrieve the noise channels of.
 *
 * @param _Channels
 * Pointer to a user-allocated array of NDXLChannel pointers. The array must contain at least ndxlGetEdgeChannelCount(_Edge) elements. The function populates the array in-place.
 *
 * @return
 * If either _Edge or _Channels are NULL, returns NDXL_ERR, otherwise populates _Channels with the noise channel pointers associated with _Edge, and returns NDXL_OK.
 *
 * @note
 * The caller retains ownership of the array, but not of the NDXLChannel objects referenced by its elements. These pointers refer to the original objects in the owning network of _Edge, so modifications through them affect those objects directly. The returned list can be used to modify or destroy its referenced objects without invalidating the array itself.
 *
 * @warning
 * The function does not verify the capacity of the supplied array. Providing an inadequately sized array can result in out-of-bounds writes and undefined behaviour.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Edge is NULL.
 * - NDXL_ARG_ERR: _Channels is NULL.
 *
 * @see ndxlGetEdgeChannelCount
*/
_NDXLIMP_ const int _CDECL ndxlGetEdgeChannels(
	_In_ _Notnull_ NDXLEdge* _Edge,
	_Out_ _Notnull_ NDXLChannel** _Channels
) _NOEXCEPT;

/**
 * @brief Gets the delay of an edge.
 *
 * @param _Edge
 * Target edge to retrieve the delay of.
 *
 * @return
 * If _Edge is NULL, returns NDXL_ERR, otherwise returns the delay of _Edge.
 *
 * @note
 * If the delay of _Edge is nonzero, ndxlGetEdgeRelativeState(_Edge) and ndxlGetVertexNeighbourState(_Vertex, _Edge) should be used instead of ndxlEdgeState(_Edge) and ndxlVertexState(ndxlVertexNeighbour(_Vertex, _Edge)) for getting the respective state vectors, as the former functions factor in the edge delay while the latter do not.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Edge is NULL.
 *
 * @see ndxlGetEdgeRelativeState
 * @see ndxlGetVertexNeighbourState
 * @see ndxlEdgeState
 * @see ndxlVertexState
*/
_NDXLIMP_ const double _CDECL ndxlGetEdgeDelay(
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

/**
 * @brief Gets the directedness of an edge.
 *
 * @param _Edge
 * Target edge to retrieve the directedness of.
 *
 * @return
 * If _Edge is NULL, returns NDXL_ERR, otherwise returns 1 if the edge is directed, and 0 if the edge is undirected.
 * 
 * @note
 * If the edge is directed, ndxlGetEdgeRelativeState(_Edge, _Vertex) should be used instead of ndxlEdgeState(_Edge), as the former factors in the directedness and returns the zero-vector if queried from the `source` vertex, while the latter does not.
 * 
 * @par Errors raised
 * - NDXL_ARG_ERR: _Edge is NULL.
 *
 * @see ndxlGetEdgeRelativeState
 * @see ndxlEdgeState
*/
_NDXLIMP_ const char _CDECL ndxlGetEdgeDirected(
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

/**
 * @brief Gets the dynamics function associated with an edge.
 *
 * @param _Edge
 * Target edge to retrieve the dynamics of.
 *
 * @return
 * If _Edge is NULL, returns NULL, otherwise returns the dynamics function associated with _Edge.
 *
 * @note
 * A return value of NULL does not necessarily indicate an error, as the dynamics function could be NULL. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Edge is NULL.
 *
 * @see ndxlGetLastErrorCode
 * @see ndxlSetEdgeDynamics
*/
_NDXLIMP_ NDXLEdgeDynamics _CDECL ndxlGetEdgeDynamics(
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

/**
 * @brief Gets the `source` vertex of an edge.
 *
 * @param _Edge
 * Target edge to retrieve the first vertex of.
 *
 * @return
 * If _Edge is NULL, returns NULL, otherwise returns the `source` vertex of _Edge.
 *
 * @note
 * If the edge is a self-loop, this is identical to calling ndxlGetEdgeSecondVertex(_Edge).
 * 
 * @par Errors raised
 * - NDXL_ARG_ERR: _Edge is NULL.
 *
 * @see ndxlGetEdgeSecondVertex
*/
_NDXLIMP_ NDXLVertex* _CDECL ndxlGetEdgeFirstVertex(
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

/**
 * @brief Gets the initial state vector of an edge.
 *
 * @param _Edge
 * Target edge to retrieve the initial state of.
 *
 * @return
 * If _Edge is NULL, returns NULL, otherwise returns the initial state vector assigned to _Edge.
 *
 * @note
 * If the initial state vector was NULL, _Edge is assigned a zero-vector of length ndxlGetEdgeOrder(_Edge), meaning this function will still return a valid pointer containing the zero-vector.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Edge is NULL.
 *
 * @see ndxlCreateEdge
 * @see ndxlGetEdgeOrder
*/
_NDXLIMP_ const double* _CDECL ndxlGetEdgeInitialState(
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

/**
 * @brief Gets the owning network of an edge.
 *
 * @param _Edge
 * Target edge to retrieve the network of.
 *
 * @return
 * If _Edge is NULL, returns NULL, otherwise returns the NDXLNetwork pointer on which _Edge was created.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Edge is NULL.
*/
_NDXLIMP_ NDXLNetwork* _CDECL ndxlGetEdgeNetwork(
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

/**
 * @brief Gets the noise function associated with an edge.
 *
 * @param _Edge
 * Target edge to retrieve the noise of.
 *
 * @return
 * If _Edge is NULL, returns NULL, otherwise returns the noise function associated with _Edge.
 *
 * @note
 * A return value of NULL does not necessarily indicate an error, as the noise function could be NULL. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Edge is NULL.
 *
 * @see ndxlGetLastErrorCode
 * @see ndxlSetEdgeNoise
*/
_NDXLIMP_ NDXLEdgeNoise _CDECL ndxlGetEdgeNoise(
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

/**
 * @brief Gets the size of an edge's state vector.
 *
 * @param _Edge
 * Target edge to retrieve the order of.
 *
 * @return
 * If _Edge is NULL, returns NDXL_ERR, otherwise returns the size of the state vector of _Edge.
 *
 * @note
 * If _State was NULL during construction, the edge is assigned a zero-initialised state vector whose length is determined by the default-state allocation behaviour described in ndxlCreateEdge().
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Edge is NULL.
 *
 * @see ndxlCreateEdge
*/
_NDXLIMP_ const size_t _CDECL ndxlGetEdgeOrder(
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

/**
 * @brief Gets the user-defined `void* _Params` of an edge.
 *
 * @param _Edge
 * Target edge to retrieve the parameters of.
 *
 * @return
 * If _Edge is NULL, returns NULL, otherwise returns the user-defined `void* _Params` of _Edge.
 *
 * @note
 * A return value of NULL does not necessarily indicate an error, as `void* _Params` could be NULL. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Edge is NULL.
 *
 * @see ndxlGetLastErrorCode
 * @see ndxlSetEdgeParams
*/
_NDXLIMP_ void* _CDECL ndxlGetEdgeParams(
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

/**
 * @brief Gets the time-delayed and directed state vector of an edge as seen by one of its vertices.
 *
 * @param _Edge
 * Target edge to retrieve the relative state of.
 *
 * @param _Vertex
 * The vertex from whose perspective the edge state vector is evaluated.
 * 
 * @return
 * If either _Edge or _Vertex are NULL, or _Edge is not connected to _Vertex, returns NULL, otherwise returns the delayed and directed state vector of the edge as seen by _Vertex.
 * - If _Edge is directed and _Vertex is the `source` vertex of _Edge, always returns the zero-vector of the edge regardless of the delay.
 * - If _Edge is delayed, returns the zero-vector of the edge for t < delay, and the current state vector for t greater than or equal to delay.
 * - If _Edge is neither directed nor delayed, returns the current state vector of _Edge.
 *
 * @note
 * If and only if both the delay of _Edge is zero and it is undirected, is this equivalent to calling ndxlEdgeState(_Edge).
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Edge is NULL.
 * - NDXL_ARG_ERR: _Vertex is NULL.
 * - NDXL_VALUE_ERR: _Edge is not connected to _Vertex.
 *
 * @see ndxlEdgeState
*/
_NDXLIMP_ const double* _CDECL ndxlGetEdgeRelativeState(
	_In_ _Notnull_ NDXLEdge* _Edge,
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/**
 * @brief Gets the `destination` vertex of an edge.
 *
 * @param _Edge
 * Target edge to retrieve the second vertex of.
 *
 * @return
 * If _Edge is NULL, returns NULL, otherwise returns the `destination` vertex of _Edge.
 *
 * @note
 * If the edge is a self-loop, this is identical to calling ndxlGetEdgeFirstVertex(_Edge).
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Edge is NULL.
 *
 * @see ndxlGetEdgeFirstVertex
*/
_NDXLIMP_ NDXLVertex* _CDECL ndxlGetEdgeSecondVertex(
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

/**
 * @brief Gets the local simulation time of an edge.
 *
 * @param _Edge
 * Target edge to retrieve the time of.
 *
 * @return
 * If _Edge is NULL, returns NDXL_ERR, otherwise returns the local simulation time of _Edge.
 *
 * @note
 * The local simulation time only updates on a successful simulation step.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Edge is NULL.
 *
 * @see @ref network_simulation "Network Simulation"
*/
_NDXLIMP_ const double _CDECL ndxlGetEdgeTime(
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

/** @}*/



/**
 * @defgroup edge_setters Edge Setters
 * @brief Setter functions for updating edge properties.
 *
 * @{
*/

/**
 * @brief Sets the callback function of an edge.
 *
 * @param _Edge
 * Target edge to update the callback of.
 *
 * @param _Callback
 * The new callback that _Edge must use. May be NULL.
 *
 * @return
 * If _Edge is NULL, returns NDXL_ERR, otherwise updates the callback of _Edge, and returns NDXL_OK.
 *
 * @warning
 * This function should never be used from within an existing callback function, as replacing a callback while it is executing can corrupt the calling stack. Additionally, this function should not be called from within a dynamics or noise function, as these may be invoked multiple times during a simulation step.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Edge is NULL.
 *
 * @see @ref network_simulation "Network Simulation"
 * @see ndxlGetEdgeCallback
*/
_NDXLIMP_ const int _CDECL ndxlSetEdgeCallback(
	_In_ _Notnull_ NDXLEdge* _Edge,
	_In_ _Maybenull_ NDXLEdgeCallback _Callback
) _NOEXCEPT;

/**
 * @brief Sets the dynamics function of an edge.
 *
 * @param _Edge
 * Target edge to update the dynamics of.
 *
 * @param _Dynamics
 * The new dynamics that _Edge must use. May be NULL.
 *
 * @return
 * If _Edge is NULL, returns NDXL_ERR, otherwise updates the dynamics of _Edge, and returns NDXL_OK.
 *
 * @warning
 * This function should never be used from within a dynamics or noise function, as these may be invoked multiple times during a simulation step. Modifications to edge properties should instead be performed from within callback functions, which are invoked after successful integration steps.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Edge is NULL.
 *
 * @see @ref network_simulation "Network Simulation"
 * @see ndxlGetEdgeDynamics
*/
_NDXLIMP_ const int _CDECL ndxlSetEdgeDynamics(
	_In_ _Notnull_ NDXLEdge* _Edge,
	_In_ _Maybenull_ NDXLEdgeDynamics _Dynamics
) _NOEXCEPT;

/**
 * @brief Sets the noise function of an edge.
 *
 * @param _Edge
 * Target edge to update the noise of.
 *
 * @param _Noise
 * The new noise that _Edge must use. May be NULL.
 *
 * @return
 * If _Edge is NULL, returns NDXL_ERR, otherwise updates the noise of _Edge, and returns NDXL_OK.
 *
 * @warning
 * This function should never be used from within a dynamics or noise function, as these may be invoked multiple times during a simulation step. Modifications to edge properties should instead be performed from within callback functions, which are invoked after successful integration steps.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Edge is NULL.
 *
 * @see @ref network_simulation "Network Simulation"
 * @see ndxlGetEdgeNoise
*/
_NDXLIMP_ const int _CDECL ndxlSetEdgeNoise(
	_In_ _Notnull_ NDXLEdge* _Edge,
	_In_ _Maybenull_ NDXLEdgeNoise _Noise
) _NOEXCEPT;

/**
 * @brief Sets the user-defined `void* _Params` of an edge.
 *
 * @param _Edge
 * Target edge to update the parameters of.
 *
 * @param _Params
 * Pointer to extra memory used by the edge. May be NULL.
 *
 * @return
 * If _Edge is NULL, returns NDXL_ERR, otherwise updates the parameters of _Edge, and returns NDXL_OK.
 *
 * @warning
 * NDXL stores the supplied `void* _Params` pointer without copying the associated data or managing its lifetime. The pointed-to memory must remain valid for as long as the edge uses it.
 *
 * This function replaces the edge's existing parameter pointer without freeing or otherwise managing the memory it previously referenced. The user remains responsible for managing both the previous and new allocations.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Edge is NULL.
 *
 * @see ndxlGetEdgeParams
*/
_NDXLIMP_ const int _CDECL ndxlSetEdgeParams(
	_In_ _Notnull_ NDXLEdge* _Edge,
	_In_ _Maybenull_ void* _Params
) _NOEXCEPT;

/** @}*/



/**
 * @defgroup edge_functions Edge Functions
 * @brief Functions for operating on edges
 *
 * @{
*/

/**
 * @brief Checks if an edge is connected to a vertex.
 *
 * @param _Edge
 * Target edge to query for vertex connectivity.
 *
 * @param _Vertex
 * The vertex to determine if _Edge is connected to.
 *
 * @return
 * If either _Edge or _Vertex are NULL, returns NDXL_ERR, otherwise returns a bitmask detailing the connection of _Edge to _Vertex:
 * - 0, if _Edge is not connected to _Vertex.
 * - 1 (bit 0), if _Edge is connected to _Vertex.
 * - 3 (bits 0 and 1), if _Edge is directed and _Vertex is the `source` vertex.
 * - 5 (bits 0 and 2), if _Edge is directed and _Vertex is the `destination` vertex.
 * - 7 (bits 0, 1, and 2) if _Edge is a self-loop.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Edge is NULL.
 * - NDXL_ARG_ERR: _Vertex is NULL.
 *
 * @see ndxlCreateEdge
*/
_NDXLIMP_ const char _CDECL ndxlEdgeConnectsVertex(
	_In_ _Notnull_ NDXLEdge* _Edge,
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/**
 * @brief Provides direct access to the internal state vector of an edge.
 *
 * @param _Edge
 * Target edge to see the state of.
 *
 * @return
 * If _Edge is NULL, returns NULL, otherwise returns a pointer to the internal state vector or, when called from within a dynamics function, the current Runge–Kutta stage vector of _Edge.
 *
 * @note
 * When called outside a dynamics function, this function returns a pointer to the edge's primary state vector. When called from within a dynamics function, it instead returns a pointer to the current intermediate Runge–Kutta stage vector. This behaviour allows the same function to be used for directly modifying an edge's state outside integration and accessing its current integration stage during dynamics evaluation.
 *
 * @warning
 * Direct modification of the returned vector is supported when it refers to the primary state vector. However, when called from within a dynamics function, the returned pointer refers to the intermediate Runge–Kutta stage vector. Modifying this vector directly is not intended for ordinary state updates and may interfere with the integration process.
 *
 * @par Errors raised
 * - NDXL_ARG_ERR: _Edge is NULL.
 *
 * @see @ref network_simulation "Network Simulation"
*/
_NDXLIMP_ double* _CDECL ndxlEdgeState(
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

/** @}*/

#ifdef __cplusplus
}
#endif

//#ifdef __cplusplus
//_NDXL_BEGIN
//
//class edge
//{
//public:
//	inline edge(NDXLEdge* _Edge) noexcept : _My_ptr(_Edge) {}
//
//	inline virtual ~edge() noexcept
//	{
//		this->_My_ptr = 0;
//	}
//
//protected:
//	NDXLEdge* _My_ptr;
//
//public:
//	inline const int destroy() noexcept
//	{
//		this->parent()._Destroy_edge_params(this->_My_ptr);
//		return ndxlDestroyEdge(&(this->_My_ptr));
//	}
//
//	inline std::vector<any>& params() noexcept
//	{
//		return (*(std::vector<any>*)(ndxlEdgeParams(this->_My_ptr)))[-1];
//	}
//
//	inline network parent() const noexcept
//	{
//		return network(ndxlGetEdgeParent(this->_My_ptr));
//	}
//};
//
//_NDXL_END
//#endif // __cplusplus

#endif // _NDXL_EDGE_