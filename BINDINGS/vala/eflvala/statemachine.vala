using GLib;

public class EflVala.ViewState
{
    protected ViewStateMachine state_machine; 
    protected Eina.Hash<string, ViewState> transition_map = null;

    public ViewState(ViewStateMachine sm)
    {
        transition_map = Eina.Hash.string_superfast<ViewState>(null);
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

    public virtual weak Elm.Object? object_get()
    {
        return null;
    }
}

public class EflVala.ViewStateMachine
{
    private Elm.Pager pager;
    private ViewState current_state;

    public ViewStateMachine(Elm.Win win)
    {
        current_state = null;

        pager = new Elm.Pager(win);
        win.resize_object_add(pager);
        pager.show();
    }

    public new void state_register(ViewState? state)
    {
        pager.content_push(state.object_get());
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
 
        pager.content_promote(((ViewState)current_state).object_get());
    }
}


