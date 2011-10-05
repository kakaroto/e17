#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var my_window = new elm.window({
		type : "main",
		label : "Calendar Demo",
		width : 320,
		height : 480,
		align : FILL_BOTH,
		elements : {
			the_background : {
				type : "background",
				weight : EXPAND_BOTH,
				align : FILL_BOTH,
				resize : true,
			},
			the_calendar : {
				type : "calendar",
				weight : EXPAND_BOTH,
				align : FILL_BOTH,
				resize : true,
				fill : true,
				weekday_names : {
					0 : "Mo",
					1 : "Tu",
					2 : "We",
					3 : "Th",
					4 : "Fr",
					5 : "Sa",
					6 : "Su",
				},
				min_year : 1900,
				max_year : 2100,
				day_selection_enabled : true,
				selected_year : 2011,
				selected_month : 1,
				selected_date : 21,
				calendar_interval : 0.5,
				marks : {
				    0 : {
					    mark_type : "holiday",
					    mark_date : 20,
					    mark_mon  : 7,
					    mark_year : 2011,
					    mark_repeat : "unique",
					},
				    1 : {
					    mark_type : "checked",
					    mark_date : 21,
					    mark_mon  : 8,
					    mark_year : 0,
					    mark_repeat : "daily",
					},
				    2 : {
					    mark_type : "checked",
					    mark_date : 22,
					    mark_mon  : 1,
					    mark_year : 0,
					    mark_repeat : "weekly",
					},
				    3 : {
					    mark_type : "clicked",
					    mark_date : 23,
					    mark_mon  : 7,
					    mark_year : 0,
					    mark_repeat : "monthly",
					},
				    4 : {
					    mark_type : "checked",
					    mark_date : 24,
					    mark_mon  : 8,
					    mark_year : 2011,
					    mark_repeat : "annualy",
					},
				},
				on_changed : function (args) {
						     print("Callback Invoked\n");
					     }
			},
		},
});
