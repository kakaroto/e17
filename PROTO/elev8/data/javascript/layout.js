
var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var my_window =  elm.realise(elm.Window({
    label : "Layout Demo",
    width : 320,
    height : 480,
    align : FILL_BOTH,
    elements : {
        the_background : elm.Background({
            weight : EXPAND_BOTH,
            align : FILL_BOTH,
            resize : true,
        }),
        layout : elm.Layout({
            weight : EXPAND_BOTH,
            align : FILL_BOTH,
            resize : true,
            theme : {
                'class' : 'layout',
                'group' : 'application',
                'style' : 'content-back-next'
            },
            content : {
                'elm.text.title' : 'Title',
                'elm.swallow.content' : elm.Icon({file: 'home'})
            }
        })
    }
}));
