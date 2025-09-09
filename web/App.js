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
			/>
			<div className="container mt-3">
				{activeTab === "Global Commands" ? (
					<GlobalCommands
						loading={loading}
						pixelState={pixelState}
						apiFetch={apiFetch}
						setPixelState={setPixelState} />
				) : (
					<Section
						name={activeTab}
						loading={loading}
						pixelState={pixelState}
						apiFetch={apiFetch}
						setPixelState={setPixelState} />
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
						{info?.sections?.map((section) => (
							<li className="nav-item" key={section}>
								<button
									className={`nav-link btn ${activeTab === section ? "active" : ""}`}
									style={{ border: "none", background: "none" }}
									onClick={() => onTabChange(section)}
								>
									{section}
								</button>
							</li>
						))}
					</ul>
				</div>

			</div>
		</nav>

	);
}

function GlobalCommands({ loading, pixelState, info, apiFetch, setPixelState }) {

	const reload = () => apiFetch("status").then(setPixelState);

	return (
		<div>
			<h3 className="d-flex justify-content-between align-items-center">
				Overview
				<button className="btn btn-sm btn-outline-secondary" disabled={loading} onClick={reload}>
					<i className="bi bi-arrow-clockwise"></i>
					{loading}
				</button>
			</h3>

			<p>Commands that affect everything.</p>
			<pre>{JSON.stringify(pixelState, (k, v) => k.length > 0 && k[0] === k[0].toLocaleUpperCase() ? undefined : v, 2)}</pre>
		</div>
	);
}

function Section({ name, loading, pixelState, info, apiFetch, setPixelState }) {
	const [section, setSection] = useState({});

	const reload = () => {
		apiFetch(name)
			.then((json) => {
				console.log(`refetched state for ${name}`, json);
				setPixelState({
					...pixelState,
					[name]: json
				})
			});
	};

	useEffect(() => {
		setSection(pixelState[name]);
	}, [name, pixelState]);

	return (
		<div>
			<h3 className="d-flex justify-content-between align-items-center">
				{name}
				<button className="btn btn-sm btn-outline-secondary" disabled={loading} onClick={reload}>
					<i className="bi bi-arrow-clockwise"></i>
				</button>
			</h3>
			<p>Controls for {name}.</p>
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
