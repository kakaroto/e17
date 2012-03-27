using GLib;

public class EflVala.ViewState
{
    protected ViewStateMachine state_machine; 
    protected Eina.Hash<string, ViewState> transition_map = null;

    public ViewState(ViewStateMachine sm)
    {
        transition_map = Eina.Hash<string, ViewState>.string_superfast(null);
        this.state_machine = sm;
    }

    public void transition_add(string transition, ViewState next)
    {
        transition_map.add(transition, next);
    }

    public ViewState? transition_handle(string condition)
    {
        return transition_map.find(condition);
    }

    public virtual void activated()
    {
    }

    public virtual void deactivated()
    {
    }

    public virtual unowned Elm.Object? object_get()
    {
        return null;
    }
}

public class EflVala.ViewStateMachine
{
    private unowned Elm.Naviframe navi;
    private ViewState current_state;

    public ViewStateMachine(Elm.Win win)
    {
        current_state = null;

        navi = Elm.Naviframe.add(win);
        win.resize_object_add(navi);
        navi.show();
    }

    public new void state_register(ViewState? state)
    {
        navi.item_push("", null, null, state.object_get(), null),
    }
    
    public void current_state_set(ViewState state)
    {
        if(current_state != null)
            current_state.deactivated();
        current_state = state;
        current_state.activated();
    }

    public void emit(string condition)
    {
        ViewState new_state = null;

        if(current_state == null) 
        {
            debug("no current state!");
            return;
        }

        new_state = current_state.transition_handle(condition);
        if(new_state != null)
            current_state_set(new_state);
 
        navi.content_promote(((ViewState)current_state).object_get());
    }
}


