/**
 * @file ndxl_vertex.h
 *
 * @brief Public API for NDXL vertex creation, state and parameter management, and nearest neighbour traversal.
 *
 * Declares the NDXLVertex object, representing a node in a network. Provides functions for vertex construction, destruction, nearest neighbour traversal, and state and parameter querying.
 *
 * @copyright Copyright (c) 2026, Ethan Codron.
 * 
 * Licensed under the GNU General Public License v3.0 only.
 * See the LICENSE file in the repository root.
*/

#ifndef _NDXL_VERTEX_
#define _NDXL_VERTEX_

#include "ndxl_channel.h"

/**
 * @defgroup vertex_dnc Dynamics, Noise, and Callbacks
 * @ingroup ndxl_vertex
 * @brief Dynamics, noise, and callback functions for vertex objects.
 *
 * @{
*/

/**
 * @brief Vertex dynamics function.
 *
 * Defines a user-supplied function that governs how the state of a vertex evolves over time.
 *
 * @param[in] _Vertex
 * The calling vertex.
 *
 * @param[out] _dY
 * Flat output vector containing the time derivative of the calling vertex. ndxlGetVertexOrder(calling_vertex) total elements must be written to it, even if those elements are zero.
 * 
 * @param[in] _Y
 * Flat input vector containing the current Runge-Kutta stage of the calling vertex. Is guaranteed to have ndxlGetVertexOrder(calling_vertex) elements.
 * 
 * @param[in] _Time
 * Local simulation time of the calling vertex, offset by a fraction of the step size corresponding to the next Runge-Kutta stage. Note that this is generally NOT equivalent to ndxlGetVertexTime(calling_vertex), which only updates on a successful step.
 *
 * @param[in] _Params
 * User-defined parameters associated with the calling vertex.
 *
 * @return
 * Integer indicating function success (zero) or failure (any nonzero value).
 * 
 * @note
 * If this function returns a nonzero value during simulation, NDXL terminates the simulation, and internally raises a @ref NDXL_SIM_ERR.
 * 
 * @warning
 * The function does not verify the number of elements written to the output array. Not writing to each element can result in garbage values or stale states being propagated. Writing beyond ndxlGetVertexOrder(calling_vertex) elements can result in out-of-bounds writes and undefined behaviour.
 * 
 * @see ndxlGetVertexOrder
*/
typedef int(*NDXLVertexDynamics)(NDXLVertex* _Vertex, double* _dY, const double* _Y, double _Time, void* _Params);

/**
 * @brief Vertex noise function.
 *
 * Defines a user-supplied function that provides the amplitude of noise, for each element in its state vector, for each independent Wiener process the vertex is driven by.
 *
 * @param[in] _Vertex
 * The calling vertex.
 *
 * @param[out] _dG
 * Flat output matrix containing the noise amplitudes, where element ij (at index i * ndxlGetVertexChannelCount(calling_vertex) + j) represents the amplitude of the j-th Wiener process on the i-th state-vector element. ndxlGetVertexOrder(calling_vertex) x ndxlGetVertexChannelCount(calling_vertex) total elements must be written to it, even if those elements are zero.
 * 
 * @param[in] _Y
 * Flat input vector containing the current Runge-Kutta stage of the calling vertex. Is guaranteed to have ndxlGetVertexOrder(calling_vertex) elements.
 * 
 * @param[in] _Time
 * Local simulation time of the calling vertex, offset by a fraction of the step size corresponding to the next Runge-Kutta stage. Note that this is generally NOT equivalent to ndxlGetVertexTime(calling_vertex), which only updates on a successful step.
 *
 * @param[in] _Params
 * User-defined parameters associated with the calling vertex.
 *
 * @return
 * Integer indicating function success (zero) or failure (any nonzero value).
 * 
 * @note
 * If this function returns a nonzero value during simulation, NDXL terminates the simulation, and internally raises a @ref NDXL_SIM_ERR.
 * 
 * @warning
 * The function does not verify the number of elements written to the output array. Not writing to each element can result in garbage values or stale states being propagated. Writing beyond ndxlGetVertexOrder(calling_vertex) x ndxlGetVertexChannelCount(calling_vertex) elements can result in out-of-bounds writes and undefined behaviour.
 * 
 * @see ndxlGetVertexOrder
 * @see ndxlGetVertexChannelCount
*/
typedef int(*NDXLVertexNoise)(NDXLVertex* _Vertex, double* _dG, const double* _Y, double _Time, void* _Params);

/**
 * @brief Vertex callback function.
 *
 * Defines a user-supplied discrete callback that is invoked during simulation of the vertex's host network.
 *
 * @param[in] _Vertex
 * The calling vertex.
 *
 * @param[in,out] _Y
 * Flat vector containing the most recently accepted state vector of the calling vertex. Is guaranteed to have ndxlGetVertexOrder(calling_vertex) elements, and may be modified in-place.
 *
 * @param[in] _Time
 * Current simulation time of the calling vertex, equivalent to ndxlGetVertexTime(calling_vertex).
 *
 * @param[in] _Params
 * User-defined parameters associated with the calling vertex.
 *
 * @return
 * Integer indicating function success (zero) or failure (any nonzero value).
 *
 * @note
 * If this function returns a nonzero value during simulation, NDXL terminates the simulation, and internally raises a @ref NDXL_SIM_ERR.
 * 
 * @warning
 * Writing beyond ndxlGetVertexOrder(calling_vertex) elements to the state vector can result in out-of-bounds writes and undefined behaviour.
 *
 * @see ndxlGetVertexOrder
*/
typedef int(*NDXLVertexCallback)(NDXLVertex* _Vertex, double* _Y, double _Time, void* _Params);

///** @}*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup vertex_creation Constructor and Destructor
 * @ingroup ndxl_vertex
 * @brief Functions for creating and destroying vertices.
 *
 * @{
*/

/**
 * @brief Creates a vertex in a network and returns a pointer to it.
 *
 * @param[in] _Network
 * Network that will own this vertex. This also dictates which noise channels can affect this vertex.
 * 
 * @param[in] _State
 * Flat initial state vector at time 0. May be NULL, in which case a zero-vector of size max(1, _Order) is assigned by default.
 * 
 * @param[in] _Order
 * Size of state vector. If _State is provided, must be nonzero.
 * 
 * @param[in] _Dynamics
 * The dynamics function describing how the state vector evolves over time. May be NULL, in which case the time derivative is zero (static vertex).
 * 
 * @param[in] _Noise
 * The noise function describing the noise amplitude over time. May be NULL, in which case _Channels must be NULL and _Nchannels must be 0.
 * 
 * @param[in] _Channels
 * Array of NDXLChannel pointers defining which noise channels affect this vertex through _Noise. May be NULL unless _Nchannels is nonzero. If provided, any element may be NULL, which is interpreted as a new channel.
 * - If _Channels is NULL and _Nchannels is 0, creates _Order new noise channels and assigns them to this vertex. The noise matrix has shape _Order x _Order.
 * - If _Channels is NULL and _Nchannels is nonzero, creates _Nchannels new noise channels and assigns them to this vertex. The noise matrix has shape _Order x _Nchannels.
 * - If any element of _Channels is NULL, creates a new noise channel for that element and assigns it to this vertex. The noise matrix has shape _Order x _Nchannels.
 * 
 * @param[in] _Nchannels
 * Size of channels array. Must be NULL if _Noise is NULL. If _Channels is provided, must match the length of the _Channels array exactly.
 * 
 * @param[in] _Callback
 * External discrete callback called once after each successful integration step. May be NULL.
 * 
 * @param[in] _Params
 * Pointer to extra memory used by the vertex. May be NULL.
 *
 * @return
 * If successful, returns a new NDXLVertex pointer, otherwise returns NULL.
 *
 * @note
 * The local simulation time of a newly created vertex is always initialised 0, and only updates on a successful integration step.
 * 
 * @note
 * Unlike network callbacks, vertex callbacks are NOT called at time 0.
 *
 * @warning
 * The function does not verify the capacity of the supplied state and channel arrays. Both arrays must contain at least _Order and _Nchannels elements, respectively, whenever they are used. Incorrect array sizes or inconsistent _Order and _Nchannels arguments can result in out-of-bounds reads and undefined behaviour.
 * 
 * @warning
 * NDXL stores the supplied `void* _Params` pointer without copying the associated data or managing its lifetime. The pointed-to memory must remain valid for as long as the vertex uses it. The user remains responsible for managing any allocations.
 *
 * @see NDXLVertexDynamics
 * @see NDXLVertexNoise
 * @see NDXLVertexCallback
 * @see ndxlDestroyVertex
*/
_NDXLIMP_ NDXLVertex* _CDECL ndxlCreateVertex(
	_In_ _Notnull_ NDXLNetwork* _Network,
	_In_reads_bytes_opt_(_Order * sizeof(double)) const double* _State,
	_In_ const size_t _Order,
	_In_ _Maybenull_ NDXLVertexDynamics _Dynamics,
	_In_ _Maybenull_ NDXLVertexNoise _Noise,
	_In_reads_bytes_opt_(_Nchannels * sizeof(NDXLChannel*)) NDXLChannel** _Channels,
	_In_ const size_t _Nchannels,
	_In_ _Maybenull_ NDXLVertexCallback _Callback,
	_In_ _Maybenull_ void* _Params
) _NOEXCEPT;

/**
 * @brief Destroys a vertex created using ndxlCreateVertex(), and zeroes the pointer.
 *
 * @param[in,out] _Vertex
 * Pointer to NDXLVertex pointer.
 *
 * @return
 * If _Vertex or *_Vertex are NULL, returns @ref NDXL_ERR, otherwise zeroes *_Vertex, and returns NDXL_OK.
 *
 * @note
 * The vertex pointer is zeroed after the associated memory is freed, so that subsequent operations using the pointer variable passed to this function can detect that the vertex has been destroyed.
 * 
 * @warning
 * Destroying a vertex also destroys all edges connected to that vertex. All pointers to the vertex and its edges become invalid. The memory associated with the relevant objects' user-defined `void* _Params` pointers is not handled by the destructors, and must be managed explicitly by the user.
 *
 * @warning
 * Only the pointer variable passed to this function is zeroed. Any other pointers referencing the destroyed vertex become invalid and must not be used.
 * 
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 * - @ref NDXL_VALUE_ERR : *_Vertex is NULL.
 * 
 * @see ndxlCreateVertex
*/
_NDXLIMP_ const int _CDECL ndxlDestroyVertex(
	_In_ _Notnull_ NDXLVertex** _Vertex
) _NOEXCEPT;

/** @}*/



/**
 * @defgroup vertex_getters Getters
 * @ingroup ndxl_vertex
 * @brief Getter functions for querying vertex properties
 *
 * @{
*/

/**
 * @brief Gets the callback function associated with a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the callback of.
 *
 * @return
 * If _Vertex is NULL, returns NULL, otherwise returns the callback function associated with _Vertex.
 * 
 * @note
 * A return value of NULL does not necessarily indicate an error, as the callback function could be NULL. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 * 
 * @see ndxlGetLastErrorCode
 * @see ndxlSetVertexCallback
*/
_NDXLIMP_ NDXLVertexCallback _CDECL ndxlGetVertexCallback(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/**
 * @brief Gets the number of noise channels affecting a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the channel count of.
 *
 * @return
 * If _Vertex is NULL, returns @ref NDXL_ERR, otherwise returns the number of noise channels affecting _Vertex.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
*/
_NDXLIMP_ const size_t _CDECL ndxlGetVertexChannelCount(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/**
 * @brief Gets a list of all the noise channels affecting a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the noise channels of.
 *
 * @param[out] _Channels
 * Pointer to a user-allocated array of NDXLChannel pointers. The array must contain at least ndxlGetVertexChannelCount(_Vertex) elements. The function populates the array in-place.
 *
 * @return
 * If either _Vertex or _Channels are NULL, returns @ref NDXL_ERR, otherwise populates _Channels with the noise channel pointers associated with _Vertex, and returns NDXL_OK.
 *
 * @note
 * The caller retains ownership of the array, but not of the NDXLChannel objects referenced by its elements. These pointers refer to the original objects in the owning network of _Vertex, so modifications through them affect those objects directly. The returned list can be used to modify or destroy its referenced objects without invalidating the array itself.
 *
 * @warning
 * The function does not verify the capacity of the supplied array. Providing an inadequately sized array can result in out-of-bounds writes and undefined behaviour.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 * - @ref  NDXL_ARG_ERR : _Channels is NULL.
 *
 * @see ndxlGetVertexChannelCount
*/
_NDXLIMP_ const int _CDECL ndxlGetVertexChannels(
	_In_ _Notnull_ NDXLVertex* _Vertex,
	_Out_ _Notnull_ NDXLChannel** _Channels
) _NOEXCEPT;

/**
 * @brief Gets the number of edges (directed and undirected) connected to a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the degree of.
 *
 * @return
 * If _Vertex is NULL, returns @ref NDXL_ERR, otherwise returns the total number of edges connected to _Vertex.
 * 
 * @note
 * Not to be confused with ndxlGetVertexDegreeIn and ndxlGetVertexDegreeOut, which return the number of edges flowing into and out of _Vertex, respectively.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 * 
 * @see ndxlGetVertexDegreeIn
 * @see ndxlGetVertexDegreeOut
 * @see ndxlCreateEdge
*/
_NDXLIMP_ const size_t _CDECL ndxlGetVertexDegree(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/**
 * @brief Gets the number of edges flowing into a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the in-degree of.
 *
 * @return
 * If _Vertex is NULL, returns @ref NDXL_ERR, otherwise returns the number of edges flowing into _Vertex. Undirected edges are counted as flowing in.
 *
 * @note
 * Not to be confused with ndxlGetVertexDegree and ndxlGetVertexDegreeOut, which return the total number of edges and number of edges flowing out of _Vertex, respectively.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 *
 * @see ndxlGetVertexDegree
 * @see ndxlGetVertexDegreeOut
 * @see ndxlCreateEdge
*/
_NDXLIMP_ const size_t _CDECL ndxlGetVertexDegreeIn(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/**
 * @brief Gets the number of edges flowing out from a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the out-degree of.
 *
 * @return
 * If _Vertex is NULL, returns @ref NDXL_ERR, otherwise returns the number of edges flowing out of _Vertex. Undirected edges are counted as flowing out.
 *
 * @note
 * Not to be confused with ndxlGetVertexDegree and ndxlGetVertexDegreeIn, which return the total number of edges and number of edges flowing into _Vertex, respectively.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 *
 * @see ndxlGetVertexDegree
 * @see ndxlGetVertexDegreeIn
 * @see ndxlCreateEdge
*/
_NDXLIMP_ const size_t _CDECL ndxlGetVertexDegreeOut(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/**
 * @brief Gets the dynamics function associated with a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the dynamics of.
 *
 * @return
 * If _Vertex is NULL, returns NULL, otherwise returns the dynamics function associated with _Vertex.
 *
 * @note
 * A return value of NULL does not necessarily indicate an error, as the dynamics function could be NULL. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 *
 * @see ndxlGetLastErrorCode
 * @see ndxlSetVertexDynamics
*/
_NDXLIMP_ NDXLVertexDynamics _CDECL ndxlGetVertexDynamics(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/**
 * @brief Gets a list of all the edges affecting a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the edges of.
 *
 * @param[out] _Edges
 * Pointer to a user-allocated array of NDXLEdge pointers. The array must contain at least ndxlGetVertexDegree(_Vertex) elements. The function populates the array in-place with all edges that are connected to _Vertex, regardless of edge directionality.
 *
 * @return
 * If either _Vertex or _Edges are NULL, returns @ref NDXL_ERR, otherwise populates _Edges with the edge pointers associated with _Vertex, and returns NDXL_OK.
 *
 * @note
 * The caller retains ownership of the array, but not of the NDXLEdge objects referenced by its elements. These pointers refer to the original objects in the owning network of _Vertex, so modifications through them affect those objects directly. The returned list can be used to modify or destroy its referenced objects without invalidating the array itself.
 *
 * @warning
 * The function does not verify the capacity of the supplied array. Providing an inadequately sized array can result in out-of-bounds writes and undefined behaviour.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 * - @ref  NDXL_ARG_ERR : _Edges is NULL.
 *
 * @see ndxlGetVertexDegree
*/
_NDXLIMP_ const int _CDECL ndxlGetVertexEdges(
	_In_ _Notnull_ NDXLVertex* _Vertex,
	_Out_ _Notnull_ NDXLEdge** _Edges
) _NOEXCEPT;

/**
 * @brief Gets the initial state vector of a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the initial state of.
 *
 * @return
 * If _Vertex is NULL, returns NULL, otherwise returns the initial state vector assigned to _Vertex.
 *
 * @note
 * If the initial state vector was NULL, _Vertex is assigned a zero-vector of length ndxlGetVertexOrder(_Vertex), meaning this function will still return a valid pointer containing the zero-vector.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 *
 * @see ndxlCreateVertex
 * @see ndxlGetVertexOrder
*/
_NDXLIMP_ const double* _CDECL ndxlGetVertexInitialState(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/**
 * @brief Gets the neighbouring vertex along a specified edge.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the neighbour of.
 * 
 * @param[in] _Edge
 * The edge along which to query the neighbouring vertex.
 *
 * @return
 * If either _Vertex or _Edge are NULL, or _Vertex is not connected to _Edge, returns NULL, otherwise returns the vertex that is connected to _Vertex by _Edge.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 * - @ref  NDXL_ARG_ERR : _Edge is NULL.
 * - @ref NDXL_VALUE_ERR : _Edge is not connected to _Vertex
 * 
 * @see ndxlCreateEdge
*/
_NDXLIMP_ NDXLVertex* _CDECL ndxlGetVertexNeighbour(
	_In_ _Notnull_ NDXLVertex* _Vertex,
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

/**
 * @brief Gets the order of a neighbouring vertex along a specified edge.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the neighbour order of.
 *
 * @param[in] _Edge
 * The edge along which to query the order of the neighbouring vertex.
 *
 * @return
 * If either _Vertex or _Edge are NULL, or _Vertex is not connected to _Edge, returns @ref NDXL_ERR, otherwise returns the order of the vertex that is connected to _Vertex by _Edge.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 * - @ref  NDXL_ARG_ERR : _Edge is NULL.
 * - @ref NDXL_VALUE_ERR : _Edge is not connected to _Vertex
 *
 * @see ndxlCreateEdge
*/
_NDXLIMP_ const size_t _CDECL ndxlGetVertexNeighbourOrder(
	_In_ _Notnull_ NDXLVertex* _Vertex,
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

/**
 * @brief Gets a list of all the nearest neighbour vertices of a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the neighbours of.
 *
 * @param[out] _Neighbours
 * Pointer to a user-allocated array of NDXLVertex pointers. The array must contain at least ndxlGetVertexDegree(_Vertex) elements. The function populates the array in-place.
 *
 * @return
 * If either _Vertex or _Neighbours are NULL, returns @ref NDXL_ERR, otherwise populates _Neighbours with the nearest neighbour vertex pointers of _Vertex, and returns NDXL_OK.
 *
 * @note
 * The caller retains ownership of the array, but not of the NDXLVertex objects referenced by its elements. These pointers refer to the original objects in the owning network of _Vertex, so modifications through them affect those objects directly. The returned list can be used to modify or destroy its referenced objects without invalidating the array itself.
 *
 * @warning
 * The function does not verify the capacity of the supplied array. Providing an inadequately sized array can result in out-of-bounds writes and undefined behaviour.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 * - @ref  NDXL_ARG_ERR : _Neighbours is NULL.
 *
 * @see ndxlGetVertexDegree
*/
_NDXLIMP_ const int _CDECL ndxlGetVertexNeighbours(
	_In_ _Notnull_ NDXLVertex* _Vertex,
	_Out_ _Notnull_ NDXLVertex** _Neighbours
) _NOEXCEPT;

/**
 * @brief Gets the time-delayed state vector of a neighbouring vertex.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the neighbour state of.
 *
 * @param[in] _Edge
 * The edge connecting _Vertex to its neighbour, the latter of which will be queried for its time-delayed state.
 * 
 * @return
 * If either _Vertex or _Edge are NULL, or _Edge is not connected to _Vertex, returns NULL, otherwise returns the delayed state vector of the neighbouring vertex to _Vertex along _Edge.
 *
 * @note
 * If the delay of _Edge is zero, this is equivalent to calling ndxlGetVertexState(ndxlGetVertexNeighbour(_Vertex, _Edge)).
 * 
 * @note
 * If the delay of _Edge is nonzero, this function uses historical states stored by _Edge to lookup the state of the neighbouring vertex at time t - ndxlGetEdgeDelay(_Edge). If the requested time does not fall exactly on a stored state, the function performs a cubic Hermite interpolation between the two surrounding states and returns the interpolated state.
 *
 * @note
 * In both cases, the returned pointer is shared, meaning it remains valid over the lifetime of the vertex, but the stored values will change on subsequent calls to this function.
 * 
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 * - @ref  NDXL_ARG_ERR : _Edge is NULL.
 * - @ref NDXL_VALUE_ERR : _Edge is not connected to _Vertex.
 *
 * @see ndxlCreateEdge
*/
_NDXLIMP_ const double* _CDECL ndxlGetVertexNeighbourState(
	_In_ _Notnull_ NDXLVertex* _Vertex,
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

/**
 * @brief Gets the owning network of a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the network of.
 *
 * @return
 * If _Vertex is NULL, returns NULL, otherwise returns the NDXLNetwork pointer on which _Vertex was created.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
*/
_NDXLIMP_ NDXLNetwork* _CDECL ndxlGetVertexNetwork(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/**
 * @brief Gets the noise function associated with a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the noise of.
 *
 * @return
 * If _Vertex is NULL, returns NULL, otherwise returns the noise function associated with _Vertex.
 *
 * @note
 * A return value of NULL does not necessarily indicate an error, as the noise function could be NULL. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 *
 * @see ndxlGetLastErrorCode
 * @see ndxlSetVertexNoise
*/
_NDXLIMP_ NDXLVertexNoise _CDECL ndxlGetVertexNoise(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/**
 * @brief Gets the size of a vertex's state vector.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the order of.
 *
 * @return
 * If _Vertex is NULL, returns @ref NDXL_ERR, otherwise returns the size of the state vector of _Vertex.
 * 
 * @note
 * If _State was NULL during construction, the vertex is assigned a zero-initialised state vector whose length is determined by the default-state allocation behaviour described in ndxlCreateVertex().
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 * 
 * @see ndxlCreateVertex
*/
_NDXLIMP_ const size_t _CDECL ndxlGetVertexOrder(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/**
 * @brief Gets the user-defined `void* _Params` of a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the parameters of.
 *
 * @return
 * If _Vertex is NULL, returns NULL, otherwise returns the user-defined `void* _Params` of _Vertex.
 *
 * @note
 * A return value of NULL does not necessarily indicate an error, as `void* _Params` could be NULL. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 *
 * @see ndxlGetLastErrorCode
 * @see ndxlSetVertexParams
*/
_NDXLIMP_ void* _CDECL ndxlGetVertexParams(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/**
 * @brief Gets the local simulation time of a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the time of.
 *
 * @return
 * If _Vertex is NULL, returns @ref NDXL_ERR, otherwise returns the local simulation time of _Vertex.
 *
 * @note
 * The local simulation time only updates on a successful simulation step.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 * 
 * @see @ref network_simulation "Network Simulation"
*/
_NDXLIMP_ const double _CDECL ndxlGetVertexTime(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/** @}*/



/**
 * @defgroup vertex_setters Setters
 * @ingroup ndxl_vertex
 * @brief Setter functions for updating vertex properties.
 *
 * @{
*/

/**
 * @brief Sets the callback function of a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to update the callback of.
 * 
 * @param[in] _Callback
 * The new callback that _Vertex must use. May be NULL.
 *
 * @return
 * If _Vertex is NULL, returns @ref NDXL_ERR, otherwise updates the callback of _Vertex, and returns NDXL_OK.
 * 
 * @warning
 * This function should never be used from within an existing callback function, as replacing a callback while it is executing can corrupt the calling stack. Additionally, this function should not be called from within a dynamics or noise function, as these may be invoked multiple times during a simulation step.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 *
 * @see @ref network_simulation "Network Simulation"
 * @see ndxlGetVertexCallback
*/
_NDXLIMP_ const int _CDECL ndxlSetVertexCallback(
	_In_ _Notnull_ NDXLVertex* _Vertex,
	_In_ _Maybenull_ NDXLVertexCallback _Callback
) _NOEXCEPT;

/**
 * @brief Sets the dynamics function of a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to update the dynamics of.
 *
 * @param[in] _Dynamics
 * The new dynamics that _Vertex must use. May be NULL.
 *
 * @return
 * If _Vertex is NULL, returns @ref NDXL_ERR, otherwise updates the dynamics of _Vertex, and returns NDXL_OK.
 *
 * @warning
 * This function should never be used from within a dynamics or noise function, as these may be invoked multiple times during a simulation step. Modifications to vertex properties should instead be performed from within callback functions, which are invoked after successful integration steps.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 *
 * @see @ref network_simulation "Network Simulation"
 * @see ndxlGetVertexDynamics
*/
_NDXLIMP_ const int _CDECL ndxlSetVertexDynamics(
	_In_ _Notnull_ NDXLVertex* _Vertex,
	_In_ _Maybenull_ NDXLVertexDynamics _Dynamics
) _NOEXCEPT;

/**
 * @brief Sets the noise function of a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to update the noise of.
 *
 * @param[in] _Noise
 * The new noise that _Vertex must use. May be NULL.
 *
 * @return
 * If _Vertex is NULL, returns @ref NDXL_ERR, otherwise updates the noise of _Vertex, and returns NDXL_OK.
 *
 * @warning
 * This function should never be used from within a dynamics or noise function, as these may be invoked multiple times during a simulation step. Modifications to vertex properties should instead be performed from within callback functions, which are invoked after successful integration steps.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 *
 * @see @ref network_simulation "Network Simulation"
 * @see ndxlGetVertexNoise
*/
_NDXLIMP_ const int _CDECL ndxlSetVertexNoise(
	_In_ _Notnull_ NDXLVertex* _Vertex,
	_In_ _Maybenull_ NDXLVertexNoise _Noise
) _NOEXCEPT;

/**
 * @brief Sets the user-defined `void* _Params` of a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to update the parameters of.
 *
 * @param[in] _Params
 * Pointer to extra memory used by the vertex. May be NULL.
 *
 * @return
 * If _Vertex is NULL, returns @ref NDXL_ERR, otherwise updates the parameters of _Vertex, and returns NDXL_OK.
 *
 * @warning
 * NDXL stores the supplied `void* _Params` pointer without copying the associated data or managing its lifetime. The pointed-to memory must remain valid for as long as the vertex uses it.
 *
 * @warning
 * This function replaces the vertex's existing parameter pointer without freeing or otherwise managing the memory it previously referenced. The user remains responsible for managing both the previous and new allocations.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 *
 * @see ndxlGetVertexParams
*/
_NDXLIMP_ const int _CDECL ndxlSetVertexParams(
	_In_ _Notnull_ NDXLVertex* _Vertex,
	_In_ _Maybenull_ void* _Params
) _NOEXCEPT;

/** @}*/



/**
 * @defgroup vertex_traversal Edge Traversal
 * @ingroup ndxl_vertex
 * @brief Functions for traversing the edges and nearest neighbours of vertices.
 *
 * @{
*/

/**
 * @brief Gets the first edge connected to a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the first edge of.
 *
 * @return
 * If _Vertex is either NULL or has no edges, returns NULL, otherwise returns the first NDXLEdge pointer connected to _Vertex.
 *
 * @note
 * This function is most commonly used in conjunction with ndxlGetVertexNextEdge, ndxlGetVertexPreviousEdge, and/or ndxlGetVertexLastEdge to iterate over a vertex's edges. The nearest neighbour vertices can subsequently be retrieved using the appropriate edge getter.
 * 
 * @note
 * A return value of NULL does not necessarily indicate an error, as the vertex could have no edges. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 *
 * @see ndxlGetLastErrorCode
 * @see ndxlGetVertexNextEdge
 * @see ndxlGetVertexPreviousEdge
 * @see ndxlGetVertexLastEdge
*/
_NDXLIMP_ NDXLEdge* _CDECL ndxlGetVertexFirstEdge(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/**
 * @brief Gets the last edge connected to a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the last edge of.
 *
 * @return
 * If _Vertex is either NULL or has no edges, returns NULL, otherwise returns the last NDXLEdge pointer connected to _Vertex.
 *
 * @note
 * This function is most commonly used in conjunction with ndxlGetVertexFirstEdge, ndxlGetVertexNextEdge, and/or ndxlGetVertexPreviousEdge to iterate over a vertex's edges. The nearest neighbour vertices can subsequently be retrieved using the appropriate edge getter.
 *
 * @note
 * A return value of NULL does not necessarily indicate an error, as the vertex could have no edges. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 *
 * @see ndxlGetLastErrorCode
 * @see ndxlGetVertexFirstEdge
 * @see ndxlGetVertexNextEdge
 * @see ndxlGetVertexPreviousEdge
*/
_NDXLIMP_ NDXLEdge* _CDECL ndxlGetVertexLastEdge(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/**
 * @brief Gets the next edge in the vertex's nearest neighbour sequence.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the next edge of.
 * 
 * @param[in] _Edge
 * Current NDXLEdge pointer in vertex's nearest neighbour sequence.
 *
 * @return
 * Returns the next NDXLEdge pointer in the vertex's edge sequence, or NULL if:
 * - _Vertex is NULL
 * - _Edge is NULL
 * - _Vertex has no edges
 * - _Edge is not connected to _Vertex
 * - _Edge is the last edge in the edge sequence of _Vertex
 *
 * @note
 * This function is most commonly used in conjunction with ndxlGetVertexFirstEdge, ndxlGetVertexPreviousEdge, and/or ndxlGetVertexLastEdge to iterate over a vertex's edges. The nearest neighbour vertices can subsequently be retrieved using the appropriate edge getter.
 *
 * @note
 * A return value of NULL does not necessarily indicate an error, as the edge could have no successors. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 * - @ref  NDXL_ARG_ERR : _Edge is NULL.
 * - NDXL_VELUE_ERR : _Edge is not connected to _Vertex.
 *
 * @see ndxlGetLastErrorCode
 * @see ndxlGetVertexFirstEdge
 * @see ndxlGetVertexPreviousEdge
 * @see ndxlGetVertexLastEdge
*/
_NDXLIMP_ NDXLEdge* _CDECL ndxlGetVertexNextEdge(
	_In_ _Notnull_ NDXLVertex* _Vertex,
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

/**
 * @brief Gets the previous edge in the vertex's nearest neighbour sequence.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the previous edge of.
 * 
 * @param[in] _Edge
 * Current NDXLEdge pointer in vertex's nearest neighbour sequence.
 *
 * @return
 * Returns the previous NDXLEdge pointer in the vertex's edge sequence, or NULL if:
 * - _Vertex is NULL
 * - _Edge is NULL
 * - _Vertex has no edges
 * - _Edge is not connected to _Vertex
 * - _Edge is the first edge in the edge sequence of _Vertex
 *
 * @note
 * This function is most commonly used in conjunction with ndxlGetVertexFirstEdge, ndxlGetVertexNextEdge, and/or ndxlGetVertexLastEdge to iterate over a vertex's edges. The nearest neighbour vertices can subsequently be retrieved using the appropriate edge getter.
 *
 * @note
 * A return value of NULL does not necessarily indicate an error, as the edge could have no predecessors. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 * - @ref  NDXL_ARG_ERR : _Edge is NULL.
 * - NDXL_VELUE_ERR : _Edge is not connected to _Vertex.
 *
 * @see ndxlGetLastErrorCode
 * @see ndxlGetVertexFirstEdge
 * @see ndxlGetVertexNextEdge
 * @see ndxlGetVertexLastEdge
*/
_NDXLIMP_ NDXLEdge* _CDECL ndxlGetVertexPreviousEdge(
	_In_ _Notnull_ NDXLVertex* _Vertex,
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

/** @}*/



/**
 * @defgroup vertex_functions Functions
 * @ingroup ndxl_vertex
 * @brief Functions for operating on vertices.
 *
 * @{
*/

/**
 * @brief Checks if an edge exists between two vertices.
 *
 * @param[in] _Source
 * `Source` vertex (vertex 1 in edge creation).
 * 
 * @param[in] _Destination
 * `Destination` vertex (vertex 2 in edge creation).
 *
 * @return
 * If either _Source or _Destination are NULL, returns @ref NDXL_ERR, otherwise returns a bitmask detailing the coupling between _Source and _Destination:
 * - 0, if no coupling exists
 * - 1 (bit 0), if at least one undirected coupling exists.
 * - 3 (bits 0 and 1), if _Source has at least one directed edge to _Destination.
 * - 5 (bits 0 and 2), if _Destination has at least one directed edge to _Source.
 * - 7 (bits 0, 1, and 2) if at least one of each type of coupling exists between _Source and _Destination.
 *
 * @note
 * This function runs in linear time proportional to ndxlGetVertexDegree(_Source). If ndxlGetVertexDegree(_Source) is larger than ndxlGetVertexDegree(_Destination), it may be worth swapping the arguments to reduce the traversal cost. However, doing so reverses the interpretation of the directed-edge flags.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Source is NULL.
 * - @ref  NDXL_ARG_ERR : _Destination is NULL.
 *
 * @see ndxlCreateEdge
*/
_NDXLIMP_ const char _CDECL ndxlVertexHasNeighbour(
	_In_ _Notnull_ NDXLVertex* _Source,
	_In_ _Notnull_ NDXLVertex* _Destination
) _NOEXCEPT;

/**
 * @brief Provides direct access to the internal state vector of a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to see the state of.
 *
 * @return
 * If _Vertex is NULL, returns NULL, otherwise returns a pointer to the internal state vector or, when called from within a dynamics function, the current Runge–Kutta stage vector of _Vertex.
 *
 * @note
 * When called outside a dynamics function, this function returns a pointer to the vertex's primary state vector. When called from within a dynamics function, it instead returns a pointer to the current intermediate Runge–Kutta stage vector. This behaviour allows the same function to be used for directly modifying a vertex's state outside integration and accessing its current integration stage during dynamics evaluation.
 * 
 * @warning
 * Direct modification of the returned vector is supported when it refers to the primary state vector. However, when called from within a dynamics function, the returned pointer refers to the intermediate Runge–Kutta stage vector. Modifying this vector directly is not intended for ordinary state updates and may interfere with the integration process.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 *
 * @see @ref network_simulation "Network Simulation"
*/
_NDXLIMP_ double* _CDECL ndxlVertexState(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/** @}*/

#ifdef __cplusplus
}
#endif

//#ifdef __cplusplus
//_NDXL_BEGIN
//
//class vertex
//{
//public:
//	inline vertex(NDXLVertex* _Vertex) noexcept : _My_ptr(_Vertex) {}
//
//	inline virtual ~vertex() noexcept
//	{
//		this->_My_ptr = 0;
//	}
//
//protected:
//	NDXLVertex* _My_ptr;
//
//	inline NDXLVertex* c_ptr() const noexcept
//	{
//		return this->_My_ptr;
//	}
//
//	inline const int destroy() noexcept
//	{
//		this->parent()._Destroy_vertex_params(this->_My_ptr);
//		for (NDXLEdge* _Edge = ndxlGetVertexFirstEdge(this->_My_ptr); _Edge; _Edge = ndxlGetVertexNextEdge(this->_My_ptr, _Edge))
//			this->parent()._Destroy_edge_params(_Edge);
//		return ndxlDestroyVertex(&(this->_My_ptr));
//	}
//
//	inline std::vector<any>& params() noexcept
//	{
//		return (*(std::vector<any>*)(ndxlVertexParams(this->_My_ptr)))[-1];
//	}
//
//	inline network parent() const noexcept
//	{
//		return network(ndxlGetVertexParent(this->_My_ptr));
//	}
//};
//
//_NDXL_END
//#endif // __cplusplus

#endif // _NDXL_VERTEX_
