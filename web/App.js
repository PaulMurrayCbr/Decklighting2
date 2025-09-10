// App.js


const { useState, useEffect, useRef } = React;

function App() {
	const [activeTab, setActiveTab] = useState("Global Commands");
	const [pixelState, setPixelState] = useState({});
	const [loading, setLoading] = useState(false);
	const loadingCount = useRef(0);
	const [error, setError] = useState(false);
	const [info, setInfo] = useState({
		effects: [],
		interpolations: [],
		ncolorranges: 0,
		sections: []
	});


	const apiFetch = async (url) => {
		if (++loadingCount.current) {
			setLoading(true);
		}
		try {
			const response = await fetch(`/api/${url}`);
			const json = await response.json();



			if (!response.ok || !json?.status || json?.status !== 200) {
				throw new Error(json?.result || response.statusText || `HTTP error ${response.status}`);
			}

			// await new Promise((resolve) => setTimeout(resolve, 1000));

			return json.result;
		}
		catch (err) {
			setError(err);
			//			throw err;
		}
		finally {
			if (!--loadingCount.current) {
				setLoading(false);
			}
		}
	};

	const apiGlobal = async (url) => {
		const json = await apiFetch(url);
		if (json)
			setPixelState(json);
	};

	const apiSection = async (section, url) => {
		const json = await apiFetch(`${section}/${url}`);
		if (json)
			setPixelState({
				...pixelState,
				[section]: json
			});

	};

	useEffect(() => {
		apiFetch("status").then(setPixelState);
		apiFetch("info").then(setInfo);
	}, []); // empty deps = run once after first render

	return (
		<div>
			<Navbar
				info={info}
				activeTab={activeTab}
				onTabChange={setActiveTab}
				loading={loading}
				pixelState={pixelState}
			/>
			<div className="container mt-3">
				{activeTab === "Global Commands" ? (
					<GlobalCommands
						loading={loading}
						pixelState={pixelState}
						info={info}
						apiGlobal={apiGlobal}
						apiSection={apiSection}
						setActiveTab={setActiveTab}
					/>
				) : (
					<Section
						name={activeTab}
						loading={loading}
						pixelState={pixelState}
						info={info}
						apiSection={apiSection}
						home={() => setActiveTab('Global Commands')}
					/>
				)}

				<div
					className="position-fixed top-0 start-0 w-100 h-100 d-flex justify-content-center align-items-center"
					style={{
						opacity: loading ? 1 : 0,
						pointerEvents: loading ? "auto" : "none", // blocks interaction only when visible
						transition: "opacity 0.3s ease-in-out", // smooth fade
						backgroundColor: 'rgba(0,0,0,0.3)',
						zIndex: 1050,   // above most Bootstrap elements
					}}
				>
					<div className="spinner-border text-light" role="status">
						<span className="visually-hidden">Loading...</span>
					</div>
				</div>

			</div>
		</div>
	);
}

function Navbar({ info, activeTab, onTabChange, loading, pixelState }) {
	return (
		<nav className="navbar navbar-expand-lg bg-body-tertiary">
			<div className="container-fluid">
				<a className="navbar-brand" href="#">Paul's Funky Lighting 2.0</a>
				<button className="navbar-toggler" type="button"
					data-bs-toggle="collapse" data-bs-target="#navbarNavAltMarkup">
					<span className="navbar-toggler-icon"></span>
				</button>
				<div className="collapse navbar-collapse" id="navbarNavAltMarkup">
					<ul className="navbar-nav me-auto mb-2 mb-lg-0">
						<li className="nav-item">
							<button
								className={`nav-link btn ${activeTab === "Global Commands" ? "active" : ""}`}
								style={{ border: "none", background: "none" }}
								onClick={() => onTabChange("Global Commands")}
							>
								Overview
							</button>
						</li>
						{info?.sections
							?.filter((section) => pixelState[section]?.mode !== 'out')
							?.map((section) => (
								<li className="nav-item" key={section}>
									<button
										className={`nav-link btn ${activeTab === section ? "active" : ""}`}
										style={{ border: "none", background: "none" }}
										onClick={() => onTabChange(section)}
									>
										{section}
									</button>
								</li>
							)
							)}
					</ul>
				</div>

			</div>
		</nav>

	);
}

function GlobalCommands({ loading, pixelState, info, apiGlobal, apiSection, setActiveTab }) {

	const debounceRef = useRef(null);
	const [brightnessState, setBrightnessState] = useState(0);

	const reload = () => apiGlobal("status");

	const togglePower = () => {
		if (pixelState?.on) {
			apiGlobal("off");
		}
		else {
			apiGlobal("on");
		}
	}

	const setBrightness = (value) => {
		setBrightnessState(value);
		if (debounceRef.current) clearTimeout(debounceRef.current);
		debounceRef.current = setTimeout(() => {
			apiGlobal(`on?brightness=${value}`);
			debounceRef.current = null;
		}, 300);
	};

	useEffect(() => {
		setBrightnessState(pixelState?.brightness ?? 0);
	}, [pixelState]);


	return (
		<div>
			<div className="d-flex justify-content-between align-items-center flex-wrap mb-3">
				<nav aria-label="breadcrumb">
					<ol className="breadcrumb">
						<li className="breadcrumb-item active" aria-current="page"><i className="fa-solid fa-house"></i> Overview</li>
					</ol>
				</nav>
				<button className="btn btn-outline-secondary btn-sm" disabled={loading} onClick={reload}>
					<i className="fa-solid fa-rotate"></i>
				</button>
			</div>

			<div className="d-flex justify-content-around align-items-center flex-wrap mb-3">
				<button
					className={`btn btn-outline-${pixelState?.on ? "success" : "danger"} btn-md`}
					onClick={togglePower}
					style={{ width: "100px", height: "100px", borderRadius: "50%" }}
				>
					<i className="fa-solid fa-power-off fa-2x"></i>
				</button>
			</div>


			<div className="d-flex align-items-center gap-3 mb-4">
				<i className="fa-solid fa-lightbulb  text-warning"></i>
				<input
					type="range"
					className="form-range"
					min="0"
					max="255"
					value={brightnessState}
					onChange={(e) => setBrightness(Number(e.target.value))}
				/>
				<span>{brightnessState}</span>
			</div>

			<GlobalLinkingButtons
				loading={loading}
				pixelState={pixelState}
				info={info}
				apiSection={apiSection}
				setActiveTab={setActiveTab}
			/>
		</div>
	);
}

function GlobalLinkingButtons({ loading, pixelState, info, apiSection, setActiveTab }) {
	const setOn = (sname) => apiSection(sname, 'on');
	const setOff = (sname) => apiSection(sname, 'off');
	const setOut = (sname) => apiSection(sname, 'out');

	let groups = [];
	let g = undefined;

	for (var sname of info?.sections ?? []) {
		const smode = pixelState[sname]?.mode ?? 'off';
		if (!g || smode === 'on' || smode === 'off') {
			const link = !!g;
			if (g) {
				g.needsLinkRight = link;
				g.rightSname = sname;
			}
			g = {
				needsLinkLeft: link,
				needsLinkRight: false,
				sections: [sname],
				state: smode,
				name: sname
			};
			groups.push(g);
		}
		else {
			g.sections.push(sname);
		}
	}

	return (
		<div className=" d-flex flex-wrap" >

			{
				groups.map((g) => {
					const on = g.state === 'on';
					const sectionStyle = `btn btn-primary flex-fill`;
					const subStyle = `btn btn-outline-primary flex-fill`;
					const linkStyle = `btn btn-outline-secondary`;

					const x = (
						<span key={`group-${g.name}`} className="btn-group d-flex flex-wrap me-2 mb-2" role="group">
							{
								g.needsLinkLeft ? (
									<button type="button" className={linkStyle} disabled={loading}
										onClick={() => setOut(g.name)}
									>
										<i className="fa-solid fa-link"></i>
									</button>
								) : null
							}

							{
								g.sections.map((s, sindex) => (
									<button key={`section-${s}`} type="button"
										className={sindex === 0 ? sectionStyle : subStyle} disabled={loading}
										onClick={() => sindex === 0 ? setActiveTab(s) : setOn(s)}
									>{s}</button>
								))
							}

							{
								g.needsLinkRight ? (
									<button type="button" className={linkStyle} disabled={loading}
										onClick={() => setOut(g.rightSname)}
									>
										<i className="fa-solid fa-link"></i>
									</button>
								) : null
							}
						</span>
					);
					return x;
				})
			}

		</div>
	);

}

function Section({ name, loading, pixelState, info, apiSection, home }) {
	const [section, setSection] = useState({});

	const reload = () => {
		apiSection(name, '');
	};

	useEffect(() => {
		setSection(pixelState[name]);
	}, [name, pixelState]);

	return (
		<div>
			<div className="d-flex justify-content-between align-items-center flex-wrap mb-3">
				<nav aria-label="breadcrumb">
					<ol className="breadcrumb">
						<li className="breadcrumb-item">
							<a href="#" onClick={home}><i className="fa-solid fa-house"></i> Overview</a>
						</li>
						<li className="breadcrumb-item active" aria-current="page">{name}</li>
					</ol>
				</nav>
				<button className="btn btn-outline-secondary btn-sm" disabled={loading} onClick={reload}>
					<i className="fa-solid fa-rotate"></i>
				</button>
			</div>

			<pre>{JSON.stringify(section, null, 2)}</pre>
		</div>
	);
}


function LoadingOverlay({ loading }) {
	return (
		<div
			className="loading-overlay"
			style={{
				opacity: loading ? 1 : 0,
				pointerEvents: loading ? "auto" : "none", // blocks interaction only when visible
				transition: "opacity 0.3s ease-in-out", // smooth fade
			}}
		>
			<div className="spinner-border text-light" role="status">
				<span className="visually-hidden">Loading...</span>
			</div>
		</div>
	);
}

// Important: expose App to the global scope
window.App = App;
