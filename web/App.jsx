// App.js


const { useState, useEffect, useRef, useCallback } = React;

function App() {
	const [activeTab, setActiveTab] = useState(SelectedPage.ofHome());
	const [pixelState, setPixelState] = useState({});
	const [loading, setLoading] = useState(false);
	const loadingCount = useRef(0);
	const [error, setError] = useState(false);
	const [recentFetch, setRecentFetch] = useState(false);
	const [info, setInfo] = useState({
		effects: [],
		interpolations: [],
		ncolorranges: 0,
		sections: []
	});


	const apiFetch = async (url) => {
		setError(null);
		if (++loadingCount.current) {
			setLoading(true);
		}
		try {
			setRecentFetch(`/api/${url}`)

			const response = await fetch(`/api/${url}`);
			const json = await response.json();

			if (!response.ok || !json?.status || json?.status !== 200) {
				throw new Error(json?.result || response.statusText || `HTTP error ${response.status}`);
			}

			return json.result;
		}
		catch (err) {
			console.log(error);
			setError(err);
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
				{activeTab.isHome() && (
					<HomePage
						loading={loading}
						pixelState={pixelState}
						info={info}
						apiGlobal={apiGlobal}
						apiSection={apiSection}
						setActiveTab={setActiveTab}
					/>
				)}
				{activeTab.isSection() && (
					<SectionPage
						name={activeTab.getSection()}
						loading={loading}
						pixelState={pixelState}
						info={info}
						apiSection={apiSection}
						home={() => setActiveTab(SelectedPage.ofHome())}
					/>
				)}
			</div>

			<Notifications
				error={error} setError={setError}
				recentFetch={recentFetch} setRecentFetch={setRecentFetch}
			/>

			<LoadingOverlay loading={loading} />
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
								className={`nav-link btn ${activeTab.isHome() ? "active" : ""}`}
								style={{ border: "none", background: "none" }}
								onClick={() => onTabChange(SelectedPage.ofHome())}
							>
								Home
							</button>
						</li>
						{info?.sections
							?.filter((section) => pixelState[section]?.mode !== 'out')
							?.map((section) => (
								<li className="nav-item" key={section}>
									<button
										className={`nav-link btn ${activeTab.isSection(section) ? "active" : ""}`}
										style={{ border: "none", background: "none" }}
										onClick={() => onTabChange(SelectedPage.ofSection(section))}
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

/* 'talking to server' overlay */
function LoadingOverlay({ loading }) {
	return (
		< div
			className="position-fixed top-0 start-0 w-100 h-100 d-flex justify-content-center align-items-center"
			style={{
				opacity: loading ? 1 : 0,
				pointerEvents: loading ? "auto" : "none", // blocks interaction only when visible
				transition: "opacity 0.3s ease-in-out", // smooth fade
				backgroundColor: 'rgba(0,0,0,0.3)',
				zIndex: 1050,   // above most Bootstrap elements
			}
			}
		>
			<div className="spinner-border text-light" role="status">
				<span className="visually-hidden">Loading...</span>
			</div>
		</div >

	);
}

/* Toast container fixed at bottom center */
function Notifications({ error, setError, recentFetch, setRecentFetch }) {
	return (
		< div
			className="toast-container position-fixed bottom-0 start-50 translate-middle-x p-3"
			style={{ zIndex: 1055 }
			}
		>
			{recentFetch && (
				<div
					className="toast align-items-center text-bg-light border-0 show"
					role="alert"
					aria-live="assertive"
					aria-atomic="true"
				>
					<div className="d-flex">
						<div className="toast-body">{recentFetch}</div>
						<button
							type="button"
							className="btn-close me-2 m-auto"
							onClick={() => setRecentFetch(null)}
						></button>
					</div>
				</div>
			)}
			{error && (
				<div
					className="toast align-items-center text-bg-danger border-0 show"
					role="alert"
					aria-live="assertive"
					aria-atomic="true"
				>
					<div className="d-flex">
						<div className="toast-body">{error?.message || e?.toString() || JSON.stringify(error)}</div>
						<button
							type="button"
							className="btn-close btn-close-white me-2 m-auto"
							onClick={() => setError(null)}
						></button>
					</div>
				</div>
			)}
		</div >
	);
}

// Important: expose App to the global scope
window.App = App;
