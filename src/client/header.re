[%%bs.raw {| require('./header.css') |}];
open Utils;

let component = ReasonReact.statelessComponent "Header";

let executeQuery event action => {
	if (ReactEventRe.Keyboard.keyCode event === 13) {
		ReactEventRe.Keyboard.preventDefault event;
		action();
	}
};

let make ::query ::onQueryUpdate ::onQueryExecute _children => {
	...component,
	render: fun _ => {
		<div id="top">
	    	<div id="top-brand">	    
				(str "Company browser")
			</div>
			<nav id="top-nav-main">
				<input className="searchBox" placeholder="Search" value=query
					onChange=onQueryUpdate
					onKeyDown=(fun event => executeQuery event onQueryExecute)/>
			</nav>
		</div>
	}
}
