using EflVala;

public class MainViewState : EflVala.ViewState
{
    private Elm.Box box;
    private Elm.Button button;

    public MainViewState(ViewStateMachine sm, Elm.Win win)
    {
        base(sm);
        
        box = new Elm.Box(win);
        box.size_hint_weight_set(1.0, 1.0);
        box.show();

        button = new Elm.Button(win);
        button.text_set("Configuration");
        button.smart_callback_add("clicked", this.on_configuration);
        box.pack_end(button);
        button.show();
    }

    public void on_configuration(Evas.Object? object, void *event_info)
    {
        state_machine.emit("to_configuration");
    }

    public override void activated()
    {
    }

    public override void deactivated()
    {
    }

    public override unowned Elm.Object? object_get()
    {
        return box;
    }
}

public class ConfigurationViewState : EflVala.ViewState
{
    private Elm.Box box;
    private Elm.Button button;

    public ConfigurationViewState(ViewStateMachine sm, Elm.Win win)
    {
        base(sm);
        
        box = new Elm.Box(win);
        box.size_hint_weight_set(1.0, 1.0);
        box.show();

        button = new Elm.Button(win);
        button.text_set("Main");
        button.smart_callback_add("clicked", this.on_main);
        box.pack_end(button);
        button.show();

    }

    public void on_main(Evas.Object? object, void *event_info)
    {
        state_machine.emit("to_main");
    }

    public override void activated()
    {
    }

    public override void deactivated()
    {
    }

    public override unowned Elm.Object? object_get()
    {
        return box;
    }
}

static int main(string[] args)
{
    Elm.init(args);

    Elm.Win win = new Elm.Win(null, "test", Elm.WinType.BASIC);
    win.title_set("Elementary StateMachine");
    win.autodel_set(true);
    win.resize(320, 320);
    win.smart_callback_add("delete-request", Elm.exit);

    Elm.Bg bg = new Elm.Bg(win);
    bg.size_hint_weight_set(1.0, 1.0);
    win.resize_object_add(bg);
    bg.show();

    EflVala.ViewStateMachine sm = new EflVala.ViewStateMachine(win);
    MainViewState main = new MainViewState(sm, win);
    ConfigurationViewState configuration = new ConfigurationViewState(sm, win);

    sm.state_register(main);
    sm.state_register(configuration);

    main.transition_add("to_configuration", configuration);
    configuration.transition_add("to_main", main);

    sm.current_state_set(main);
 
    win.resize(320, 320);
 
    win.show();

    Elm.run();
    Elm.shutdown();

    return 0;
}
