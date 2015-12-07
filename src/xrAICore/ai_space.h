////////////////////////////////////////////////////////////////////////////
//  Module      : ai_space.h
//  Created     : 12.11.2003
//  Modified    : 12.11.2003
//  Author      : Dmitriy Iassenev
//  Description : AI space class
////////////////////////////////////////////////////////////////////////////

#pragma once
#include "xrCore/xrCore.h"

class CGameGraph;
class CGameLevelCrossTable;
class CLevelGraph;
class CGraphEngine;
class CEF_Storage;
class CALifeSimulator;
class CCoverManager;
class CScriptEngine;
class CPatrolPathStorage;
class moving_objects;

namespace doors {
    class manager;
} // namespace doors

class CAI_Space {
private:
    friend class CALifeSimulator;
    friend class CALifeGraphRegistry;
    friend class CALifeSpawnRegistry;
    friend class CALifeSpawnRegistry;
    friend class CLevel;

private:
    CGameGraph                          *m_game_graph;
    CLevelGraph                         *m_level_graph;
    CGraphEngine                        *m_graph_engine;
    CEF_Storage                         *m_ef_storage;
    CALifeSimulator                     *m_alife_simulator;
    CCoverManager                       *m_cover_manager;
    CPatrolPathStorage                  *m_patrol_path_storage;
    moving_objects                      *m_moving_objects;
    doors::manager                      *m_doors_manager;

private:
            void                        load                    (LPCSTR level_name);
            void                        unload                  (bool reload = false);
            void                        patrol_path_storage_raw (IReader &stream);
            void                        patrol_path_storage     (IReader &stream);
            void                        set_alife               (CALifeSimulator *alife_simulator);
            void                        game_graph              (CGameGraph *game_graph);
            void LoadCommonScripts();
            void RegisterScriptClasses();

public:
                                        CAI_Space               ();
    virtual                             ~CAI_Space              ();
            void                        init                    ();
            void SetupScriptEngine();
    IC      CGameGraph                  &game_graph             () const;
    IC      CGameGraph                  *get_game_graph         () const;
    IC      CLevelGraph                 &level_graph            () const;
    IC      const CLevelGraph           *get_level_graph        () const;
            const CGameLevelCrossTable  &cross_table            () const;
            const CGameLevelCrossTable  *get_cross_table        () const;
    IC      const CPatrolPathStorage    &patrol_paths           () const;
    IC      CEF_Storage                 &ef_storage             () const;
    IC      CGraphEngine                &graph_engine           () const;
    IC      const CALifeSimulator       &alife                  () const;
    IC      const CALifeSimulator       *get_alife              () const;
    IC      const CCoverManager         &cover_manager          () const;
    // XXX: [ai] delete
    IC      CScriptEngine               &script_engine          () const;
    IC      moving_objects              &moving_objects         () const;
    IC      doors::manager&             doors                   () const;

#ifdef DEBUG
            void                        validate                (const u32          level_id) const;
#endif
};

IC  CAI_Space   &ai ();

extern CAI_Space *g_ai_space;

IC	CGameGraph					&CAI_Space::game_graph				() const
{
	VERIFY					(m_game_graph);
	return					(*m_game_graph);
}

IC	CGameGraph					*CAI_Space::get_game_graph			() const
{
	return					(m_game_graph);
}

IC	CLevelGraph		&CAI_Space::level_graph							() const
{
	VERIFY					(m_level_graph);
	return					(*m_level_graph);
}

IC	const CLevelGraph	*CAI_Space::get_level_graph					() const
{
	return					(m_level_graph);
}

IC	CEF_Storage					&CAI_Space::ef_storage				() const
{
	VERIFY					(m_ef_storage);
	return					(*m_ef_storage);
}

IC	CGraphEngine				&CAI_Space::graph_engine			() const
{
	VERIFY					(m_graph_engine);
	return					(*m_graph_engine);
}

IC	const CALifeSimulator		&CAI_Space::alife					() const
{
	VERIFY					(m_alife_simulator);
	return					(*m_alife_simulator);
}

IC	const CALifeSimulator		*CAI_Space::get_alife				() const
{
	return					(m_alife_simulator);
}

IC	const CCoverManager			&CAI_Space::cover_manager			() const
{
	VERIFY					(m_cover_manager);
	return					(*m_cover_manager);
}
// XXX: [ai] delete
IC	CScriptEngine				&CAI_Space::script_engine			() const
{
	VERIFY					(GlobalEnv.ScriptEngine);
	return					(*GlobalEnv.ScriptEngine);
}

IC	const CPatrolPathStorage &CAI_Space::patrol_paths				() const
{
	VERIFY					(m_patrol_path_storage);
	return					(*m_patrol_path_storage);
}

IC	moving_objects &CAI_Space::moving_objects						() const
{
	VERIFY					(m_moving_objects);
	return					(*m_moving_objects);
}

IC	doors::manager& CAI_Space::doors								() const
{
	VERIFY					(m_doors_manager);
	return					(*m_doors_manager);
}

IC	CAI_Space &ai													()
{
	if (!g_ai_space) {
		g_ai_space			= xr_new<CAI_Space>();
		g_ai_space->init	();
	}
	return					(*g_ai_space);
}
