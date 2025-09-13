
const { useState, useEffect, useRef, useCallback } = React;


function SectionPage({ name, loading, pixelState, info, apiSection, home }) {

	const [section, setSection] = useState({});
	const [selected, setSelected] = useState({ main: true, color: 0 });
	const [nColors, setNColors] = useState(0);

	useEffect(() => {
		setSection(pixelState[name] ?? {});
	}, [pixelState, name])

	useEffect(() => {
		const n = info?.effectInfo[section?.effect]?.ncolorranges ?? 0;
		setNColors(n);

		if (!selected.main && selected.color >= n) {
			selectMainPage();
		}
	}, [section, info])

	const pageApiSection = useCallback((url) => {
		apiSection(name, url);
	}, [name]);

	const pageColorSection = useCallback((url) => {
		apiSection(name, `color/${selected.color}${url}`);
	}, [name, selected.color]);

	function reload() {
		pageApiSection('');
	};

	function selectMainPage() {
		setSelected({
			...selected,
			main: true
		});
	};

	function selectColor(c) {
		setSelected({
			...selected,
			main: false,
			color: c
		});
	};

	function mainPillActive() {
		return selected.main ? 'active' : '';
	}

	function colorPillActive(i) {
		return !selected.main && selected.color === i ? 'active' : '';
	}

	function colorPillName(i) {
		return `Color${nColors < 2 ? '' : ' ' + String.fromCharCode(65 + i)}`
	}

	return (
		<div>
			<div className="d-flex justify-content-between align-items-center flex-wrap mb-3">
				<nav aria-label="breadcrumb">
					<ol className="breadcrumb">
						<li className="breadcrumb-item">
							<a href="#" onClick={home}><i className="fa-solid fa-house"></i> Home</a>
						</li>
						<li className="breadcrumb-item active" aria-current="page">{name}</li>
					</ol>
				</nav>
				<button className="btn btn-outline-secondary btn-sm" disabled={loading} onClick={reload}>
					<i className="fa-solid fa-rotate"></i>
				</button>
			</div>

			<ul className="nav nav-pills mb-3">
				<li className="nav-item">
					<button className={`nav-link ${mainPillActive()}`} onClick={selectMainPage} >Main</button>
				</li>

				{Array.from({ length: nColors }).map((_, i) => (
					<li className="nav-item" key={"color-pick-pill-" + i}>
						<button className={`nav-link ${colorPillActive(i)}`}
							onClick={() => selectColor(i)}
						>{colorPillName(i)}</button>
					</li>
				))}
			</ul>

			{selected.main && (
				<SectionMain
					section={section}
					info={info}
					apiSection={pageApiSection}
				/>
			)}
			{!selected.main && (
				<SectionColor
					color={section.color[selected.color]}
					info={info}
					apiColor={pageColorSection}
				/>
			)}

		</div>
	);
}

class Debouncer {
	debounce = undefined;
	ms = 300;
	setMethod = () => { }
	apiCall = () => { }

	constructor(ms, setMethod, apiCall) {
		this.ms = ms;
		this.setMethod = setMethod;
		this.apiCall = apiCall;
	}

	set(value) {
		this.setMethod(value);
		if (this.debounce) clearTimeout(this.debounce);
		this.debounce = setTimeout(() => {
			this.apiCall(value);
			this.debounce = undefined;
		}, this.ms);
	}
}

function SectionMain({ section, info, apiSection }) {

	const togglePower = () => {
		if (section?.mode === "on") {
			apiSection("off");
		}
		else {
			apiSection("on");
		}
	}

	const [densityState, setDensityState] = useState(0);

	const apiSectionRef = useRef(apiSection);
	useEffect(() => {
		apiSectionRef.current = apiSection;
	}, [apiSection]);

	function apiSectionStable(url) {
		aapiSectionRef.current(url);
	}

	const densityDebouncer = useRef(new Debouncer(100,
		setDensityState,
		(value) => apiSectapiSectionStableion(`set?density=${value}`)
	));

	const activeEffectPill = useRef(null);

	useEffect(() => {
		setDensityState(section?.density ?? 1);

		if (activeEffectPill.current) {
			activeEffectPill.current.scrollIntoView({ behavior: "smooth", inline: "center" });
		}
	}, [section]);

	function effectPillActive(name) {
		return section?.effect === name ? 'active' : '';
	}

	function setEffect(value) {
		apiSection(`set?effect=${value}`);
	}

	return (
		<div>
			<div className="d-flex justify-content-around align-items-center flex-wrap mb-3">
				<button
					className={`btn btn-outline-${section?.mode === "on" ? "success" : "danger"} btn-md`}
					onClick={togglePower}
					style={{ width: "100px", height: "100px", borderRadius: "50%" }}
				>
					<i className="fa-solid fa-power-off fa-2x"></i>
				</button>
			</div>



			<div className="d-flex align-items-center ">
				<input
					type="range"
					className="form-range"
					min="1"
					max="10"
					value={densityState}
					onChange={(e) => densityDebouncer.current.set(Number(e.target.value))}
				/>
			</div>

			<div className="d-flex justify-content-between w-100 mb-4">

				{Array.from({ length: 11 }).map((_, i) => (
					<i
						key={`bulb-${i}`}
						className="fa-solid fa-lightbulb"
						style={{ color: i % densityState == 0 ? 'gold' : 'darkgray' }}></i>
				))}

			</div>

			<div className="overflow-auto scrollable-pills">
				<ul className="nav nav-pills flex-nowrap">

					{info?.effects?.map((ename) => (
						<li key={`effect-pill-${ename}`}
							className="nav-item">
							<button className={`nav-link ${effectPillActive(ename)}`}
								onClick={() => setEffect(ename)}
								ref={section?.effect === ename ? activeEffectPill : null}
							>{ename}</button>
						</li>
					))}
				</ul>
			</div>
		</div>

	);
}


function toRgb(value) {
	// Remove leading #
	value = value.replace(/^#/, '');

	if (value.length !== 6) {
		console.log(`weird color value ${value}`)
		return;
	}

	const r = parseInt(value.slice(0, 2), 16);
	const g = parseInt(value.slice(2, 4), 16);
	const b = parseInt(value.slice(4, 6), 16);

	return `r=${r}&g=${g}&b=${b}`;

}

function SectionColor({ color, info, apiColor }) {

	const apiColorRef = useRef(apiColor);
	useEffect(() => {
		apiColorRef.current = apiColor;
	}, [apiColor]);

	function apiColorStable(url) {
		apiColorRef.current(url);
	}

	const [fromColorState, setFromColorState] = useState(0);
	const fromColorDebouncer = useRef(new Debouncer(100, setFromColorState,
		(value) => apiColorStable(`/from?${toRgb(value)}`)
	));

	const [toColorState, setToColorState] = useState(0);
	const toColorDebouncer = useRef(new Debouncer(100, setToColorState,
		(value) => apiColorStable(`/to?${toRgb(value)}`)
	));

	const [biasState, setBiasState] = useState(500);
	const biasDebouncer = useRef(new Debouncer(100, setBiasState,
		(value) => apiColorStable(`?bias=${value / 10000}`)
	));

	const SLIDER_RESOLUTION = 10000;

	const CYCLE_SPEED_MIN = 10;
	const CYCLE_SPEED_MAX = 1000 * 60 * 60 * 24;

	const FRAME_RATE_MIN = 50;
	const FRAME_RATE_MAX = 1000 * 60;

	const CYCLE_SPEED_MIN_LOG = Math.log(CYCLE_SPEED_MIN);
	const CYCLE_SPEED_MAX_LOG = Math.log(CYCLE_SPEED_MAX);
	const CYCLE_SPEED_S2V_SCALE = (CYCLE_SPEED_MAX_LOG - CYCLE_SPEED_MIN_LOG) / SLIDER_RESOLUTION;
	const CYCLE_SPEED_V2S_SCALE = SLIDER_RESOLUTION / (CYCLE_SPEED_MAX_LOG - CYCLE_SPEED_MIN_LOG);


	const FRAME_RATE_MIN_LOG = Math.log(FRAME_RATE_MIN);
	const FRAME_RATE_MAX_LOG = Math.log(FRAME_RATE_MAX);
	const FRAME_RATE_S2V_SCALE = (FRAME_RATE_MAX_LOG - FRAME_RATE_MIN_LOG) / SLIDER_RESOLUTION;
	const FRAME_RATE_V2S_SCALE = SLIDER_RESOLUTION / (FRAME_RATE_MAX_LOG - FRAME_RATE_MIN_LOG);

	function clamp(value, min, max) {
		return Math.min(Math.max(value, min), max);
	}

	function cs_sliderToValue(pos) {
		return clamp(
			Math.exp(CYCLE_SPEED_MIN_LOG + CYCLE_SPEED_S2V_SCALE * pos),
			CYCLE_SPEED_MIN, CYCLE_SPEED_MAX);
	}

	function cs_valueToSlider(val) {
		return clamp((Math.log(val) - CYCLE_SPEED_MIN_LOG) * CYCLE_SPEED_V2S_SCALE,
			0, SLIDER_RESOLUTION);
	}

	function fr_sliderToValue(pos) {
		return clamp(
			Math.exp(FRAME_RATE_MIN_LOG + FRAME_RATE_S2V_SCALE * pos),
			FRAME_RATE_MIN, FRAME_RATE_MAX);
	}

	function fr_valueToSlider(val) {
		return clamp((Math.log(val) - FRAME_RATE_MIN_LOG) * FRAME_RATE_V2S_SCALE,
			0, SLIDER_RESOLUTION);
	}

	const CYCLE_SPEED_1s_SLIDER = cs_valueToSlider(1000);
	const FRAME_RATE_1s_SLIDER = fr_valueToSlider(1000);


	function threeDigits(n) {
		if (Math.abs(n) > 100)
			return "" + Math.round(n);
		else if (Math.abs(n) > 10)
			return "" + (Math.round(n * 10) / 10);
		else if (Math.abs(n) > 1)
			return "" + (Math.round(n * 100) / 100);
		else
			return "" + (Math.round(n * 1000) / 1000);
	}

	const [cycleSpeedState, setCycleSpeedState] = useState(500);
	const cycleSpeedDebouncer = useRef(new Debouncer(100, setCycleSpeedState,
		(value) => apiColorStable(`?cycleSpeed=${cs_sliderToValue(value)}`)
	));

	const [frameRateState, setFrameRateState] = useState(500);
	const frameRateDebouncer = useRef(new Debouncer(100, setFrameRateState,
		(value) => apiColorStable(`?frameDuration=${fr_sliderToValue(value)}`)
	));

	const activeInterpolationPill = useRef(null);

	function interpolationPillActive(name) {
		return color?.interpolation === name ? 'active' : '';
	}


	const timeUnits = [
		{ u: 'd', t: 24 * 60 * 60 * 1000 },
		{ u: 'h', t: 60 * 60 * 1000 },
		{ u: 'm', t: 60 * 1000 }
	];

	function timeString(t) {
		if (Math.abs(t) < 1000)
			return threeDigits(1000 / t) + ' fps'

		let s = '';

		let used = 0;
		let remaining = t;
		for (const tu of timeUnits) {
			if (tu.t > t) continue;
			if (used > 0) {
				s += ' ';
			}
			const amt = Math.floor(remaining / tu.t);

			s += amt + tu.u;
			remaining -= amt * tu.t;
			if (++used >= 2) break;

		}
		if (used == 0) {
			// don't need ms, because we will get fps instead.
			s += threeDigits(remaining / 1000) + 's'
		}
		else if (used == 1) {
			s += ' ';
			s += Math.floor(remaining / 1000) + 's'
		}

		return s;
	}


	useEffect(() => {
		setFromColorState(color?.from ?? '#000000');
		setToColorState(color?.to ?? '#000000');
		setBiasState(color?.bias * 10000 ?? 500);
		setCycleSpeedState(cs_valueToSlider(color?.cycleSpeed) ?? 0);
		setFrameRateState(fr_valueToSlider(color?.frameDuration) ?? 0);
		if (activeInterpolationPill.current) {
			activeInterpolationPill.current.scrollIntoView({ behavior: "smooth", inline: "center" });
		}

	}, [color])

	return (
		<div>
			<div className="row mb-3">
				<label className="col-sm-3 col-form-label">From/To</label>
				<div className="col-sm-9">
					<input
						type="color"
						className="form-control-color"
						value={fromColorState}
						onChange={e => fromColorDebouncer.current.set(e.target.value)}
					/>
					<input
						type="color"
						className="form-control-color"
						value={toColorState}
						onChange={e => toColorDebouncer.current.set(e.target.value)}
					/>
				</div>
			</div>

			<div className="row mb-3">
				<label className="col-sm-3 col-form-label">Transition</label>
				<div className="col-sm-9">



					<div className="overflow-auto scrollable-pills">
						<ul className="nav nav-pills flex-nowrap">
							{info?.interpolations?.map((iname) => (
								<li key={`interpolation-pill-${iname}`}
									className="nav-item">
									<button className={`nav-link ${interpolationPillActive(iname)}`}
										onClick={() => apiColor(`?interpolation=${iname}`)}
										ref={color?.interpolation === iname ? activeInterpolationPill : null}
									>{iname}</button>
								</li>
							))}
						</ul>
					</div>


				</div>
			</div>

			<div className="row mb-3">
				<label className="col-sm-3 col-form-label">Seamless</label>
				<div className="col-sm-9">
					<input
						type="checkbox"
						className="form-check-input"
						checked={color.seamless}
						onChange={e => {
							const value = e.target.checked;
							apiColor(`?seamless=${value}`)
						}}
					/>
				</div>
			</div>

			<div className="row mb-3">
				<label className="col-sm-3 col-form-label">Bias</label>
				<div className="col-sm-9">
					<input
						type="range"
						className="form-range"
						min="1"
						max="9999"
						value={biasState}
						onChange={e => biasDebouncer.current.set(parseFloat(e.target.value))}
					/>
				</div>
			</div>

			<hr />

			<div className="row ">
				<label className="col-sm-3 col-form-label">Animating</label>
				<div className="col-sm-9">
					<input
						type="checkbox"
						className="form-check-input"
						checked={color.animating}
						onChange={e => {
							const value = e.target.checked;
							apiColor(`?animating=${value}`)
						}}
					/>
				</div>
			</div>

			<div className="row mb-3">
				<label className="col-sm-3 col-form-label">Cycle speed</label>
				<div className="col-sm-9 d-flex align-items-center">
					<input
						type="range"
						className="form-range"
						min="0"
						max={SLIDER_RESOLUTION}
						value={cycleSpeedState}
						onChange={e => cycleSpeedDebouncer.current.set(parseFloat(e.target.value))}
						style={{ flexGrow: 1 }}  // slider fills available space
					/>
					<span className="ms-3" style={{ whiteSpace: 'nowrap' }}>
						{timeString(cs_sliderToValue(cycleSpeedState))}
					</span>
				</div>
			</div>

			<div className="row mb-3">
				<label className="col-sm-3 col-form-label">Frame rate</label>
				<div className="col-sm-9 d-flex align-items-center">
					<input
						type="range"
						className="form-range"
						min="0"
						max={SLIDER_RESOLUTION}
						value={frameRateState}
						onChange={e => frameRateDebouncer.current.set(parseFloat(e.target.value))}
						style={{ flexGrow: 1 }}  // slider fills available space
					/>
					<span className="ms-3" style={{ whiteSpace: 'nowrap' }}>
						{timeString(fr_sliderToValue(frameRateState))}
					</span>
				</div>
			</div>
		</div>

	);
}

window.SectionPage = SectionPage;